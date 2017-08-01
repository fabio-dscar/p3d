#include <BDPT.h>

using namespace Photon;

/* ======================================================================
        Bidirectional PT utility functions
========================================================================*/

// Convert from solid angle density to area density pdf
Float Photon::toAreaDensity(Float pdf, const PathVertex& v0, const PathVertex& v1) {
    const Vec3 w = v1.point() - v0.point();

    Float lenSqr = w.lengthSqr();
    if (lenSqr == 0)
        return 0;

    Float dot = 1;
    if (v1.isSurface())
        dot = absDot(v1.geoNormal(), normalize(w));

    return pdf * dot / lenSqr;
}

Float Photon::shadingNormalFactor(const SurfaceEvent& evt, const Vec3& wo, const Vec3& wi) {
    // Apply shading normal correction
    const Normal shadNormal = evt.sFrame.normal();
    const Normal geoNormal  = evt.normal;

    Float num   = absDot(shadNormal, wo) * absDot(geoNormal, wi);
    Float denom = absDot(shadNormal, wi) * absDot(geoNormal, wo);

    if (denom == 0)
        return 0;

    return num / denom;
}

Color Photon::geomTerm(const Scene& scene, const PathVertex& v0, const PathVertex& v1) {
    // Compute geometric term
    Vec3 w = v0.point() - v1.point();
    Float lenSqr = w.lengthSqr();
    if (lenSqr == 0)
        return Color::BLACK;
    w.normalize();

    Float G = 1;
    if (v0.isSurface())
        G *= absDot(v0.normal(), w);

    if (v1.isSurface())
        G *= absDot(v1.normal(), w);
       
    if (G == 0)
        return Color::BLACK;

    // Compute visibility term
    if (!v0.visibility(scene, v1))
        return Color::BLACK;

    return G / lenSqr;
}

struct ProbPair {
    Float pdfFwd;
    Float pdfBack;
    bool delta;
};

Float Photon::calcMisWeight(const Scene& scene, const Path& cameraPath, const Path& lightPath, const PathVertex& sampled, uint32 t, uint32 s) {
    if ((s + t) == 2)
        return 1;

    Float sum = 0;
    auto remap0 = [](Float v) -> Float { return v == 0 ? 1 : v; };

    std::vector<ProbPair> cam(t);
    //cam.resize(t);

    std::vector<ProbPair> light(s);
    //light.resize(s);
    
    for (int32 i = t - 1; i >= 0; --i) {
        const PathVertex& camVert = cameraPath[i];
        cam[i].pdfFwd  = remap0(camVert.pdfFwd);
        cam[i].pdfBack = remap0(camVert.pdfBack);
        cam[i].delta   = camVert.isDelta();
    }

    for (int32 i = s - 1; i >= 0; --i) {
        const PathVertex& lightVert = lightPath[i];
        light[i].pdfFwd  = remap0(lightVert.pdfFwd);
        light[i].pdfBack = remap0(lightVert.pdfBack);
        light[i].delta   = lightVert.isDelta();
    }

    PathVertex lv, cv, prevLv, prevCv;

    if (s > 0)
        lv = lightPath[s - 1];

    if (t > 0)
        cv = cameraPath[t - 1];

    if (s == 1) {
        lv = sampled;
        light[s - 1].pdfBack = remap0(sampled.pdfBack);
        light[s - 1].pdfFwd  = remap0(sampled.pdfFwd);
        light[s - 1].delta   = sampled.isDelta();
    } else if (t == 1) {
        cv = sampled;
        cam[t - 1].pdfBack = remap0(sampled.pdfBack);
        cam[t - 1].pdfFwd  = remap0(sampled.pdfFwd);
        cam[t - 1].delta   = sampled.isDelta();
    }

    if (s > 1)
        prevLv = lightPath[s - 2];

    if (t > 1)
        prevCv = cameraPath[t - 2];

    /*if (t > 0)
        cam[t - 1].delta = false;
    
    if (s > 0)
        light[s - 1].delta = false;*/

    // Compute camera vertices reverse PDFs
    if (t > 0) {
        if (s > 0) {
            if (s == 1)
                cam[t - 1].pdfBack = lv.evalPdf(cv);
            else
                cam[t - 1].pdfBack = lv.evalPdf(prevLv, cv);
        } else {
            cam[t - 1].pdfBack = cv.evalLightDistrPdf(scene, prevCv);
        }
    }


    if (t > 1) {
        if (s > 0) {
            cam[t - 2].pdfBack = cv.evalPdf(lv, prevCv);
        } else {
            cam[t - 2].pdfBack = cv.evalPdf(prevCv);
        }
    }

    // Compute light vertices reverse PDFs
    if (s > 0) {
        if (t == 1)
            light[s - 1].pdfBack = cv.evalPdf(lv);
        else
            light[s - 1].pdfBack = cv.evalPdf(prevCv, lv);
    }

    if (s > 1) {
        light[s - 2].pdfBack = lv.evalPdf(cv, prevLv);
    }


    /*std::cout << "Camera path:" << std::endl;
    for (int32 i = t - 1; i > 0; --i)
        std::cout << "v[" << i << "]: (" << cam[i].pdfBack << ", " << cam[i].pdfFwd << ")   w = " << remap0(cam[i].pdfBack) / remap0(cam[i].pdfFwd) << std::endl;

    std::cout << "Light path:" << std::endl;
    for (int32 i = s - 1; i >= 0; --i)
        std::cout << "v[" << i << "]: (" << light[i].pdfBack << ", " << light[i].pdfFwd << ")   w = " << remap0(light[i].pdfBack) / remap0(light[i].pdfFwd) << std::endl;
        */

    Float ri = 1;
    for (int32 i = t - 1; i > 0; --i) {
        Float p = remap0(cam[i].pdfBack) / remap0(cam[i].pdfFwd);
        ri *= p * p;
        if (!cam[i].delta && !cam[i - 1].delta)
            sum += ri;
    }

    ri = 1;
    for (int32 i = s - 1; i >= 0; --i) {
        Float p = remap0(light[i].pdfBack) / remap0(light[i].pdfFwd);
        ri *= p * p;
        uint32 idx = i > 0 ? i - 1 : 0;
        if (!light[i].delta && !light[idx].delta)
            sum += ri;
    }

    return 1.0 / (1.0 + sum);
}


/* ======================================================================
        PathVertex member functions
========================================================================*/

PathVertex PathVertex::createCameraVertex(const Camera& cam, const Ray& ray, const Color& beta) {
    PathVertex v;
    v.type = VertexType::CAMERA;
    v.cam  = EndVertex<Camera>(&cam, ray);
    v.beta = beta;

    return v;
}

PathVertex PathVertex::createLightVertex(const Light& light, const Ray& ray, const Color& beta, const Normal& n, Float pdf) {
    PathVertex v;
    v.type   = VertexType::LIGHT;
    v.light  = EndVertex<Light>(&light, ray, n);
    v.pdfFwd = pdf;
    v.beta   = beta;

    return v;
}

PathVertex PathVertex::createSurfaceVertex(const SurfaceEvent& evt, const Color& beta, Float pdf, const PathVertex& prev) {
    PathVertex v;
    v.type   = VertexType::SURFACE;
    v.evt    = evt;
    v.beta   = beta;
    v.pdfFwd = toAreaDensity(pdf, prev, v);

    return v;
}

void PathVertex::setEvent(VertexType vertType, const SurfaceEvent& event) {
    beta    = Color(1);
    evt     = event;
    type    = vertType;
    pdfFwd  = 0;
    pdfBack = 0;
}

bool PathVertex::connectible() const {
    return !isDelta();
}

bool PathVertex::isLight() const {
    return (type == VertexType::LIGHT) || 
           (type == VertexType::SURFACE && evt.obj->isLight());
}

bool PathVertex::isSurface() const {
    //return type == SURFACE;
    return !(normal() == Normal());
}

bool PathVertex::isDelta() const {
    if (type == LIGHT)
        return light->isDelta();

    return (type == SURFACE && 
           (evt.obj->bsdf() && evt.obj->bsdf()->isType(SPECULAR)));
}

bool PathVertex::isEndpoint() const {
    return isLight() || type == CAMERA;
}

const Light* PathVertex::getLight() const {
    if (type == LIGHT)
        return light.ptr;
    else if (evt.obj->isLight())
        return (Light*)evt.obj->areaLight();

    return nullptr;
}

const Camera* PathVertex::getCam() const {
    if (type == CAMERA)
        return cam.ptr;

    return nullptr;
}

const SurfaceEvent* PathVertex::getSurface() const {
    if (type == SURFACE)
        return &evt;

    return nullptr;
}

const Point3& PathVertex::point() const {   
    switch (type) {
        case SURFACE:
            return evt.point;
            break;
        case CAMERA:
            return cam.evt.point; 
            break;
        case LIGHT:
            return light.evt.point;
            break;
        default:
            return evt.point;
    } 
}

const RayEvent& PathVertex::event() const {
    switch (type) {
        case SURFACE:
            return evt;
            break;
        case CAMERA:
            return cam.evt;
            break;
        case LIGHT:
            return light.evt;
            break;
        default:
            return evt;
    }
}

const Normal& PathVertex::geoNormal() const {
    switch (type) {
        case SURFACE:
            return evt.normal;
            break;
        case CAMERA:
            return cam.evt.normal;
            break;
        case LIGHT:
            return light.evt.normal;
            break;
        default:
            return evt.normal;
    }
}

Normal PathVertex::normal() const {
    switch (type) {
        case SURFACE:
            return evt.sFrame.normal();
            break;
        case CAMERA:
            return cam.evt.normal;
            break;
        case LIGHT:
            return light.evt.normal;
            break;
        default:
            return evt.normal;
    }
}

bool PathVertex::visibility(const Scene& scene, const PathVertex& next) const {
    const Ray visRay = event().spawnRay(next.point());
    if (scene.isOccluded(visRay))
        return false;

    return true;
}

Color PathVertex::evalL(const PathVertex& ref) const {
    if (!isLight())
        return Color::BLACK;

    // Direction from this vertex to reference vertex
    Vec3 pToRef = ref.point() - point();
    if (pToRef.lengthSqr() == 0)
        return Color::BLACK;

    // Return emission from light
    if (type == VertexType::LIGHT)
        return light->evalL(evt, normalize(pToRef));

    // Return emission from surface
    return evt.emission(normalize(pToRef));
}

Color PathVertex::evalBsdf(const PathVertex& ref, Transport mode) const {
    if (type != VertexType::SURFACE)
        return Color::BLACK;

    if (!evt.obj->bsdf() || evt.obj->bsdf()->isType(BSDFType::NONE))
        return Color::BLACK;

    // Direction from this vertex to reference vertex
    Vec3 pToRef = ref.point() - point();
    if (pToRef.lengthSqr() == 0)
        return Color::BLACK;
    pToRef.normalize();

    // Build BSDFSample and evaluate BSDF
    BSDFSample bs = BSDFSample(evt, pToRef, mode);
    Color f = evt.obj->bsdf()->eval(bs);

    // Avoid light leaks
    if (dot(evt.normal, pToRef) * Frame::cosTheta(bs.wi) <= 0)
        return Color::BLACK;

    // Perform shading normal correction
    if (!f.isBlack() && mode == IMPORTANCE)
        f *= shadingNormalFactor(evt, evt.toWorld(bs.wo), pToRef);

    return f;
}

Float PathVertex::evalPdf(const PathVertex& prev, const PathVertex& next) const {
    // Evaluate PDF for surfaces
    if (type != SURFACE)
        return 0;

    // Compute world direction to previous
    Vec3 wprev = prev.point() - point();
    if (wprev.lengthSqr() == 0)
        return 0;

    // Compute world direction to next
    Vec3 wnext = next.point() - point();
    if (wnext.lengthSqr() == 0)
        return 0;

    // Evaluate PDF with computed directions
    BSDFSample bs(evt, normalize(wnext), RADIANCE);
    bs.wo = evt.toLocal(normalize(wprev));

    Float pdf = evt.obj->bsdf()->evalPdf(bs);

    return toAreaDensity(pdf, *this, next);
}

Float PathVertex::evalPdf(const PathVertex& next) const {
    // Eval PDF for end vertices
    if (!isEndpoint())
        return 0;

    Vec3 wnext = next.point() - point();
    if (wnext.lengthSqr() == 0)
        return 0;

    Float pdf = 0;
    if (type == CAMERA) {
        const Ray ray = Ray(point(), normalize(wnext));

        pdf = cam->pdfWe(ray);
    } else if (isLight()) {         
        const Light* l = getLight();
        const PositionSample ps(event());
        const DirectionSample ds(normalize(wnext));

        pdf = l->pdfEmitDirection(ps, ds);

        // const DirectSample ds(next.event(), event());
        // pdf = l->pdfDirect(ds);
    }

    return toAreaDensity(pdf, *this, next);
}

Float PathVertex::evalLightDistrPdf(const Scene& scene, const PathVertex& next) const {
    if (!isLight())
        return 0;

    // Evaluate position PDF
    PositionSample ps(event());
    //Float pdfPos = light->pdfPosition(ps);
    DirectSample ds(next.event(), event());
    Float pdfPos = light->pdfDirect(ds);

    DiscretePdf1D* distr = scene.lightDistribution();
    if (!distr)
        return pdfPos;

    const Light* light = getLight();

    // Get index for this light in light array
    auto lights = scene.getLights();
    auto iter   = std::find(lights.begin(), lights.end(), light);
    uint32 idx  = std::distance(lights.begin(), iter);

    // Evaluate distribution PDF
    Float pdfDistr = distr->pdf(idx);

    return pdfDistr * pdfPos;
}

/* ======================================================================
		BidirPathTracer member functions 
 ========================================================================*/

void BidirPathTracer::renderTile(uint32 tId, uint32 tileId) const {
	const ImageTile& tile = _tiles[tileId];
	Sampler& sampler = *tile.samp.get();

	const Camera& camera = _scene->getCamera();
	for (uint32 y = 0; y < tile.h; ++y) {
		for (uint32 x = 0; x < tile.w; ++x) {
			Point2ui pixel(x + tile.x, y + tile.y);

			sampler.start(pixel);

			// Iterate samples per pixel
			Color color = Color::BLACK;
			for (uint32 s = 0; s < sampler.spp(); ++s) {
                sampler.startSample(s);

				// Generate camera path
				Path cameraPath = createPath(PATH_CAMERA, _maxDepth + 2, sampler);

				// Generate light path
				Path lightPath = createPath(PATH_LIGHT, _maxDepth + 1, sampler);

				// Perform all connections between the two paths
				Color Li = connectPaths(cameraPath, lightPath, sampler);

                // Record sample on camera's film
                camera.film().addColorSample(pixel.x, pixel.y, Li);

                color += Li;
			}

			// Use box filter for preview
			color /= sampler.spp();
			camera.film().addPreviewSample(pixel.x, pixel.y, color);
		}
	}
}

Path BidirPathTracer::createPath(PathType type, uint32 maxDepth, Sampler& sampler) const {
	const Camera& camera = _scene->getCamera();

	Float pdf = 0;
	Ray ray;
	Path path(type, maxDepth);
	Color beta(1);

	if (type == PATH_LIGHT) {
		// Choose a light according to sampling strategy
		Float lightPdf = 1;
		const Light* l = _scene->sampleLightPdf(sampler.next1D(), &lightPdf);

		// Sample position on light
		PositionSample ps;
		l->samplePosition(sampler.next2D(), &ps);

		if (ps.pdf == 0)
			return path;

		// Sample direction from sampled position
		DirectionSample ds;
		Color Le = l->sampleEmitDirection(sampler.next2D(), ps, &ds);

		if (ds.pdf == 0 || Le.isBlack())
			return path;

		// Set starting ray
		ray = Ray(ps.pos, ds.wo);

		path[0] = PathVertex::createLightVertex(*l, ray, Le, ps.normal(), lightPdf * ps.pdf);

		// Set starting throughput and pdf
		beta = Le * absDot(ps.normal(), ray.dir()) / (lightPdf * ps.pdf * ds.pdf);
		pdf  = ds.pdf;
	
    }
	else if (type == PATH_CAMERA) {
		// Set starting ray
		ray = camera.primaryRay(sampler.pixel(), sampler);

		path[0] = PathVertex::createCameraVertex(camera, ray, Color(1));

		// Set starting pdf
		pdf = camera.pdfWe(ray);
	}

	// Build rest of path from starting ray
	path.performWalk(*_scene, sampler, ray, beta, pdf);

	return path;
}