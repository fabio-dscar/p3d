#pragma once

#include <Integrator.h>
#include <Ray.h>
#include <Shape.h>
#include <AreaLight.h>
#include <Camera.h>

#include <Scene.h>

namespace Photon {

    class Camera;
    class Light;
    class PathVertex;

    Float toAreaDensity(Float pdf, const PathVertex& v0, const PathVertex& v1);
    Float shadingNormalFactor(const SurfaceEvent& evt, const Vec3& wo, const Vec3& wi);
    Color geomTerm(const Scene& scene, const PathVertex& v0, const PathVertex& v1);
    Float calcMisWeight();

    enum VertexType {
        SURFACE, CAMERA, LIGHT
    };

    template<typename T>
    class EndVertex {
    public:
        RayEvent evt;
        const T* ptr;

        EndVertex(const T* ptr, const RayEvent& event)
            : evt(event), ptr(ptr) { }

        EndVertex(const T* ptr, const Ray& ray)
            : ptr(ptr) {

            evt.wo    = ray.dir();
            evt.point = ray.origin();
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
            : type(CAMERA), beta(beta), cam(cam, ray) { }

        void setEvent(VertexType vertType, const SurfaceEvent& event);

        const Point3&   point()  const;
        const Normal&   normal() const;
        const RayEvent& event()  const;
        
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

            Float pdfFwd = pdf;
            Float pdfBack = 0;

            const BSDF* bsdf = nullptr;

            uint32 depth = 1;
            while (depth < maxDepth) {
                PathVertex& prev = verts[depth - 1];
                PathVertex& vert = verts[depth];

                SurfaceEvent event;
                if (!scene.intersectRay(subPath, &event)) {

                    // If tracing from the camera
                    if (type == PATH_CAMERA) {
                        // Escaped ray
                    }

                    break;
                }

                // Fetch intersection's BSDF
                bsdf = event.obj->bsdf();
                if (!bsdf || bsdf->isType(BSDFType::NONE))
                    break;  // Leave if null bsdf (e.g. light sources)

                // Create surface vertex
                vert = PathVertex::createSurfaceVertex(event, beta, pdfFwd, prev);
                ++numVerts;

                // Sample BSDF for scattered direction
                BSDFSample bs = BSDFSample(event);
                Color f = bsdf->sample(sampler.next2D(), &bs);

                // Leave if no contribution from sampled direction
                if (bs.pdf == 0 || f.isBlack())
                    break;

                // Update the throughput
                beta *= f * Frame::absCosTheta(bs.wi) / bs.pdf;

                // Evaluate backwards pdf
                BSDFSample rev = bs;
                std::swap(rev.wi, rev.wo);
                rev.type = BSDFType::ALL;
                pdfBack = bsdf->evalPdf(rev);

                if (hasType(bs.type, BSDFType(SPECULAR))) {
                    pdfBack = 0;
                    pdfFwd = 0;
                }
                
                // If tracing from a light, perform shading normal correction
                if (type == PATH_LIGHT)
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

    using namespace std::placeholders;

    class BidirPathTracer : public Integrator {
    public:
        BidirPathTracer(const Scene& scene, uint32 spp = 1)
            : Integrator(scene), _maxDepth(16), _spp(spp) {
            
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

        void renderTile(uint32 tId, uint32 tileId) const {         
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

                        // Generate camera path
                        Path cameraPath = createPath(PATH_CAMERA, _maxDepth + 2, sampler);

                        // Generate light path
                        Path lightPath = createPath(PATH_LIGHT, _maxDepth + 1, sampler);

                        // Perform all connections between the two paths
                        color += connectPaths(cameraPath, lightPath, sampler);

                    }

                    // Filter result
                    color /= _spp;

                    // Record sample on camera's film
                    camera.film().addColorSample(pixel.x, pixel.y, color);
                }
            }

        }

        Path createPath(PathType type, uint32 maxDepth, Sampler& sampler) const {
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
                beta *= Le * absDot(ps.normal(), ray.dir()) / (lightPdf * ps.pdf * ds.pdf);
                pdf = ds.pdf;  

            } else if (type == PATH_CAMERA) {
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

        Color connectPaths(const Path& cameraPath, const Path& lightPath, Sampler& sampler) const {
            const Camera& cam = _scene->getCamera();
            
            Color L = Color::BLACK;
            for (uint32 t = 1; t <= cameraPath.numVerts; ++t) {
                for (uint32 s = 0; s <= lightPath.numVerts; ++s) {
                    int32 depth = t + s - 2;

                    if ((s == 1 && t == 1) || depth < 0 || depth > _maxDepth)
                        continue;

                    StratResult strat = computeStrategy(sampler, cameraPath, lightPath, t, s);

                    if (t != 1) {
                        L += strat.L;
                    } else {
                        if (!strat.L.isBlack()) {
                            Point2 raster = strat.raster;

                            ; // Add splat
                        }                      
                    }
                }
            }

            return Color::BLACK;
        }

        StratResult computeStrategy(Sampler& sampler, const Path& cameraPath, const Path& lightPath, uint32 t, uint32 s) const {            
            const Camera& cam = _scene->getCamera();
   
            StratResult ret;

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
                const Point3 hit   = ds.hitPoint();
                const Ray camRay   = Ray(hit, -ds.wi);
                PathVertex camVert = PathVertex::createCameraVertex(cam, camRay, Lc / ds.pdf);

                // Check if point intersects view plane
                // Get its raster position
                Point2 raster;
                if (!cam.toRaster(camRay, &raster))
                    return Color::BLACK;
                ret.raster = raster;

                // Compute radiance associated with connection
                L += lv.beta * camVert.beta * lv.evalBsdf(camVert, IMPORTANCE);
                L *= absDot(lv.normal(), ds.wi);

                // Compute visibility term
                if (!L.isBlack() && !lv.visibility(*_scene, camVert))
                    return Color::BLACK;

            } else if (s == 1) {
                const PathVertex& cv = cameraPath[t - 1];

                if (!cv.isSurface())
                    return Color::BLACK;

                // Choose a light source using the light distribution
                Float lightPdf = 1;
                const Light* light = _scene->sampleLightPdf(sampler.next1D(), &lightPdf);

                if (!light || lightPdf == 0)
                    return Color::BLACK;

                // Sample the light source
                const Point2 ls = sampler.next2D();
                const Point2 bs = sampler.next2D();

                // Sample direction to light source
                DirectSample ds(cv.event());
                Color Le = light->sampleDirect(sampler.next2D(), &ds);

                if (ds.pdf == 0 || Le.isBlack())
                    return Color::BLACK;

                // Create light vertex
                Float pdf  = ds.pdf * lightPdf;
                Color beta = Le / pdf;

                const Point3 hit   = ds.hitPoint();
                const Ray lightRay = Ray(hit, -ds.wi); 
                PathVertex lightVert = PathVertex::createLightVertex(*light, lightRay, beta, ds.normal, pdf);

                // Compute radiance associated with connection
                L += cv.beta * lightVert.beta * cv.evalBsdf(lightVert, RADIANCE);
                L *= absDot(cv.normal(), ds.wi);

                // Compute visibility term
                if (!L.isBlack() && !cv.visibility(*_scene, lightVert))
                    return Color::BLACK;

                    /*{
                    const Ray visRay = cv.evt.spawnRay(ds.wi, ds.dist);
                    if (_scene->isOccluded(visRay))
                        return Color::BLACK;
                        }*/

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

            // TODO: Caluculate MIS
            Float mis = calcMisWeight();
            ret.L   = L * mis;
            ret.mis = mis;

            return ret;
        }

    private:
        uint32 _spp;
        uint32 _maxDepth;
    };

    
}