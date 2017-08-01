#pragma once

#include <Integrator.h>
#include <Ray.h>
#include <Shape.h>
#include <AreaLight.h>
#include <Camera.h>

#include <Scene.h>

using namespace std::placeholders;

namespace Photon {

    class Camera;
    class Light;
    class PathVertex;
    class Path;

    Float toAreaDensity(Float pdf, const PathVertex& v0, const PathVertex& v1);
    Float shadingNormalFactor(const SurfaceEvent& evt, const Vec3& wo, const Vec3& wi);
    Color geomTerm(const Scene& scene, const PathVertex& v0, const PathVertex& v1);
    Float calcMisWeight(const Scene& scene, const Path& cameraPath, const Path& lightPath, const PathVertex& sampled, uint32 t, uint32 s);

    enum VertexType {
        SURFACE, CAMERA, LIGHT
    };

    template<typename T>
    class EndVertex {
    public:
        RayEvent evt;
        const T* ptr;

        EndVertex(const T* ptr, const RayEvent& event)
            : evt(event), ptr(ptr), { }

        EndVertex(const T* ptr, const Ray& ray)
            : ptr(ptr) {

            evt.wo     = ray.dir();
            evt.point  = ray.origin();
            evt.normal = Normal(0);
        }

        EndVertex(const T* ptr, const Ray& ray, const Normal& n)
            : ptr(ptr) {

            evt.wo     = ray.dir();
            evt.point  = ray.origin();
            evt.normal = n;
        }

        const T* operator->() const {
            return ptr;
        }
    };

    class PathVertex {
    public:
        Color beta;    // Throughput
        Float pdfFwd;
        Float pdfBack;
        VertexType type;

        union {
            SurfaceEvent evt;
            EndVertex<Camera> cam;
            EndVertex<Light> light;
        };
        
        PathVertex() : beta(1), pdfFwd(0), pdfBack(0) {}
        PathVertex(const Camera* cam, const Ray& ray, const Color& beta) 
            : type(CAMERA), beta(beta), cam(cam, ray), pdfFwd(0), pdfBack(0) { }

        void setEvent(VertexType vertType, const SurfaceEvent& event);

        const Point3&   point()     const;     
        const RayEvent& event()     const;
        const Normal&   geoNormal() const;
        Normal normal() const;
        
        bool connectible() const;
        bool isLight()     const;
        bool isSurface()   const;
        bool isDelta()     const;
        bool isEndpoint()  const;

        const Light* getLight() const;
        const Camera* getCam() const;
        const SurfaceEvent* getSurface() const;

        Color evalL(const PathVertex& ref) const;
        Color evalBsdf(const PathVertex& ref, Transport mode) const;
        Float evalPdf(const PathVertex& prev, const PathVertex& next) const;
        Float evalPdf(const PathVertex& next) const;
        Float evalLightDistrPdf(const Scene& scene, const PathVertex& next) const;

        bool visibility(const Scene& scene, const PathVertex& next) const;

        static PathVertex createCameraVertex(const Camera& cam, const Ray& ray, const Color& beta);
        static PathVertex createLightVertex(const Light& light, const Ray& ray, const Color& beta, const Normal& n, Float pdf);
        static PathVertex createSurfaceVertex(const SurfaceEvent& evt, const Color& beta, Float pdf, const PathVertex& prev);
    };

    enum PathType {
        PATH_CAMERA, PATH_LIGHT
    };

    class Path {
    public:
        const PathType type;
        const uint32 maxDepth;
        uint32 numVerts;
        std::vector<PathVertex> verts;

        Path(PathType type, uint32 maxDepth) : type(type), maxDepth(maxDepth) {
            numVerts = 1;
            verts.resize(maxDepth);          
        }

        PathVertex& operator[](uint32 v) {
            if (v >= maxDepth)
                return verts[verts.size() - 1];

            return verts[v];
        }

        const PathVertex& operator[](uint32 v) const {
            if (v >= maxDepth)
                return verts[verts.size() - 1];

            return verts[v];
        }

        Color evalL(uint32 v) const {
            const PathVertex& vert = verts[v - 1];
            if (vert.isLight())
                return vert.beta * vert.evalL(verts[v - 2]);

            return Color::BLACK;
        }
   
        void performWalk(const Scene& scene, Sampler& sampler, const Ray& wo, Color beta, Float pdf) {
            Ray subPath = wo;

            Float pdfFwd  = pdf;
            Float pdfBack = 0;

            const BSDF* bsdf = nullptr;
            Transport transp = (type == PATH_LIGHT) ? IMPORTANCE : RADIANCE;

            uint32 depth = 1;
            while (depth < maxDepth) {
                PathVertex& prev = verts[depth - 1];
                PathVertex& vert = verts[depth];

                SurfaceEvent event;
                if (!scene.intersectRay(subPath, &event)) {
                    // If tracing from the camera
                    if (transp == RADIANCE) {
                        // Escaped ray
                    }

                    break;
                }

                // Avoid light leaks
                if (dot(event.normal, -subPath.dir()) * Frame::cosTheta(event.wo) <= 0)
                    break;

                // Create surface vertex
                vert = PathVertex::createSurfaceVertex(event, beta, pdfFwd, prev);
                ++numVerts;

                // Fetch intersection's BSDF
                bsdf = event.obj->bsdf();
                if (!bsdf || bsdf->isType(BSDFType::NONE))
                    break;  // Leave if null bsdf (e.g. light sources)

                // Sample BSDF for scattered direction
                BSDFSample bs = BSDFSample(event, transp);
                Color f = bsdf->sample(sampler.next2D(), &bs);

                // Leave if no contribution from sampled direction
                if (bs.pdf == 0 || f.isBlack())
                    break;

                // Update the throughput
                beta *= f * Frame::absCosTheta(bs.wi) / bs.pdf;
                
                // Evaluate backwards pdf
                BSDFSample rev = bs;
                rev.type = BSDFType::ALL;
                rev.invert();
                
                pdfBack  = bsdf->evalPdf(rev);
                pdfFwd   = bs.pdf;

                if (hasType(bs.type, BSDFType(SPECULAR))) {
                    pdfBack = 0;
                    pdfFwd  = 0;
                }
                
                // If tracing from a light, perform shading normal correction
                if (transp == IMPORTANCE)
                    beta *= shadingNormalFactor(event, event.toWorld(bs.wo), event.toWorld(bs.wi));

                // Spawn a new ray in the sampled direction
                subPath = event.spawnRay(event.toWorld(bs.wi));

                prev.pdfBack = toAreaDensity(pdfBack, vert, prev);

                ++depth;
            }


        }

    };





    struct StratResult {
        Color  L;
        Point2 raster;
        Float  mis;

        StratResult() 
            : L(Color::BLACK), raster(), mis(0) { }

        StratResult(const Color& c)
            : L(c), raster(), mis(0) { }
    };

#define DEBUG(str) std::cout << str << std::endl;

    class BidirPathTracer : public Integrator {
    public:
        BidirPathTracer(const Scene& scene, uint32 spp = 64)
            : Integrator(scene), _maxDepth(8), _spp(spp) {
            
        }

        void initialize() {
            Integrator::initialize();
        }

        void startRender(EndCallback endCallback = EndCallback()) {
            // Add task for drawing tiles in parallel
            _renderTask = Threading::Workers->pushTask(
                std::bind(&BidirPathTracer::renderTile, this, _2, _1),
                uint32(_tiles.size()),
                endCallback
            );
        }

    private:

		void renderTile(uint32 tId, uint32 tileId) const;

		Path createPath(PathType type, uint32 maxDepth, Sampler& sampler) const;

        Color connectPaths(const Path& cameraPath, const Path& lightPath, Sampler& sampler) const {
            const Camera& cam = _scene->getCamera();
            
            // Record features on features buffer
            if (cameraPath.numVerts > 1) {
                const PathVertex& v0 = cameraPath[0];
                const PathVertex& v1 = cameraPath[1];
                if (v1.isSurface()) {
                    FeaturesRecord rec;

                    Point2ui pixel = sampler.pixel();
                    rec.raster = Point2(pixel.x, pixel.y);                   
                    rec.dist   = dist(v0.event().point, v1.event().point);
                    rec.normal = v1.normal();

                    cam.film().addFeatureSample(rec);
                }
            }

            //std::cout << "-> Pixel: (" << sampler.pixel().x << ", " << sampler.pixel().y << ")" << std::endl;

            // Compute contributions of all paths
            Color L = Color::BLACK;
            for (uint32 t = 1; t <= cameraPath.numVerts; ++t) {
                for (uint32 s = 0; s <= lightPath.numVerts; ++s) {
                    int32 depth = t + s - 2;

					// Do not connect the camera and light endpoints directly
					// Also check if the length of the path is within bounds
                    if ((s == 1 && t == 1) || depth < 0 || depth > _maxDepth)
                        continue;

                    StratResult strat = computeStrategy(sampler, cameraPath, lightPath, t, s);

                    if (t != 1) {
                        L += strat.L * strat.mis;
                    } else {
                        // Add splat
                        if (!strat.L.isBlack())                 
                            cam.film().addSplatSample(strat.raster, strat.L * strat.mis);                    
                    }

                    /*std::cout << "Strat (s, t) = (" << s << ", " << t << ")" << std::endl;
                    std::cout << "L: (" << strat.L.r << ", " << strat.L.g << ", " << strat.L.b << ")" << std::endl;
                    std::cout << "MIS weight: " << strat.mis << std::endl;
                    std::cout.flush();

                    std::cin.get();*/

#ifdef PHOTON_DEBUG
                    if (std::isinf(strat.L.r) || std::isnan(strat.L.r) ||
                        std::isinf(strat.L.g) || std::isnan(strat.L.g) ||
                        std::isinf(strat.L.b) || std::isnan(strat.L.b)) {
                        DEBUG("Radiance not a number/infinite!!");
                    }

                    if (strat.L.r < 0 || strat.L.g < 0 || strat.L.b < 0)
                        DEBUG("Radiance below zero!!");
#endif

                }
            }

            return L;
        }

        StratResult computeStrategy(Sampler& sampler, const Path& cameraPath, const Path& lightPath, uint32 t, uint32 s) const {            
            const Camera& cam = _scene->getCamera();
   
            PathVertex sampled;
            StratResult ret;

            if (t > 1 && s != 0 && cameraPath[t - 1].type == VertexType::LIGHT)
                return Color::BLACK;

            Color L = Color::BLACK;
            if (s == 0) {
                // Only camera path vertices
                L += cameraPath.evalL(t);
 
            } else if (t == 1) {
                // Only use light path vertices
                // Connect to a point in the camera

                const PathVertex& lv = lightPath[s - 1];

                if (!lv.connectible())
                    return Color::BLACK;

                // Sample point in camera
                DirectSample ds(lv.event());
                Color Lc = cam.sampleDirect(sampler.next2D(), &ds);
                
                if (ds.pdf == 0 || Lc.isBlack())
                    return Color::BLACK;
                   
                // Create camera vertex and compute L
                const Color beta = Lc / ds.pdf;
                const Point3 hit = ds.hitPoint();
                const Ray camRay = Ray(hit, -ds.wi);	

                PathVertex camVert = PathVertex::createCameraVertex(cam, camRay, beta);

                // Check if point intersects view plane
                // Get its raster position
                Point2 raster;
                if (!cam.toRaster(camRay, &raster))
                    return Color::BLACK;
                ret.raster = raster;

                // Compute radiance associated with connection
                L += lv.beta * camVert.beta * lv.evalBsdf(camVert, IMPORTANCE);
                if (lv.isSurface())
                    L *= absDot(lv.normal(), ds.wi);

                // Compute visibility term
                if (!L.isBlack() && !lv.visibility(*_scene, camVert))
                    return Color::BLACK;

                sampled = camVert;
            } else if (s == 1) {
                // Only use camera path vertices
                // Connect to a point at one light

                const PathVertex& cv = cameraPath[t - 1];

                if (!cv.connectible())
                    return Color::BLACK;

                // Choose a light source using the light distribution
                Float lightPdf = 1;
                const Light* light = _scene->sampleLightPdf(sampler.next1D(), &lightPdf);

                if (!light || lightPdf == 0)
                    return Color::BLACK;

                // Sample direction to light source
                DirectSample ds(cv.event());
                Color Le = light->sampleDirect(sampler.next2D(), &ds);

                if (ds.pdf == 0 || Le.isBlack())
                    return Color::BLACK;

                // Create light vertex
                Float pdf  = ds.pdf * lightPdf;
                Color beta = Le / lightPdf;

                const Point3 hit   = ds.hitPoint();
                const Ray lightRay = Ray(hit, -ds.wi); 
                PathVertex lightVert = PathVertex::createLightVertex(*light, lightRay, beta, ds.normal, pdf);
               
                PositionSample ps(lightVert.event());
                Float posPdf = light->pdfPosition(ps);
                lightVert.pdfFwd = posPdf * lightPdf;

                // Compute radiance associated with connection
                L += cv.beta * lightVert.beta * cv.evalBsdf(lightVert, RADIANCE);
                if (cv.isSurface())
                    L *= absDot(cv.normal(), ds.wi);

                // Compute visibility term
                if (!L.isBlack() && !cv.visibility(*_scene, lightVert))
                    return Color::BLACK;

                sampled = lightVert;
            } else {
                const PathVertex& cv = cameraPath[t - 1];
                const PathVertex& lv = lightPath[s - 1];

                if (cv.connectible() && lv.connectible()) {
                    L += lv.beta * cv.beta * lv.evalBsdf(cv, IMPORTANCE) * cv.evalBsdf(lv, RADIANCE);

                    if (!L.isBlack())
                        L *= geomTerm(*_scene, lv, cv);
                }
            }
    
            if (L.isBlack())
                return Color::BLACK;

            Float mis = calcMisWeight(*_scene, cameraPath, lightPath, sampled, t, s);
            ret.L   = L;
            ret.mis = mis;

            return ret;
        }

    private:
        uint32 _spp;
        int32 _maxDepth;
    };

    
}