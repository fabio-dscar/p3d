#include <WhittedRayTracer.h>

#include <Camera.h>
#include <Ray.h>
#include <Scene.h>

#include <AreaLight.h>
#include <BSDF.h>
#include <Records.h>

using namespace Photon;
using namespace Photon::Threading;
using namespace std::placeholders;

void WhittedRayTracer::startRender(EndCallback endCallback) {
    // Add task for drawing tiles in parallel
    _renderTask = Threading::Workers->pushTask(
        std::bind(&WhittedRayTracer::renderTile, this, _2, _1),
        uint32(_tiles.size()),
        endCallback
    );
}

// This is called by different threads
void WhittedRayTracer::renderTile(uint32 tId, uint32 tileId) const {
    const ImageTile& tile = _tiles[tileId];

    const Camera& camera = _scene->getCamera();
    for (uint32 y = 0; y < tile.h; ++y) {
        for (uint32 x = 0; x < tile.w; ++x) {
            Vec2ui pixel(x + tile.x, y + tile.y);

            // Get color from ray
            const uint32 SAMPLES = 8; // 16
            const uint32 LENS_SAMPLES = 4;

            // Get jittered samples
            std::vector<Point2> pixelSamples;
            std::vector<Point2> lensSamples;
  
            jittered2DArray(_random, SAMPLES, pixelSamples);
            jittered2DArray(_random, SAMPLES, lensSamples);

            // Compute ray tracing
            Color color(0);        
            for (uint32 i = 0; i < pixelSamples.size(); ++i) {
                Color lensEstimate(0);
                Point2 ps = pixelSamples[i];
                Point2 ls = lensSamples[i];

                Point2 rand = Point2(ls.x, ls.y);
                Point2 p = Point2(pixel.x + ps.x, pixel.y + ps.y);

                Ray ray = camera.primaryRay(p, rand);

                color += traceRay(ray, 1.0, 1, pixel);
            }

            // Filter result
            color /= SAMPLES;

            // Record sample on camera's film
            camera.film().addColorSample(pixel.x, pixel.y, color);

            /*Color color(0);
            for (uint32 p = 0; p < SAMPLES; p++) {
                for (uint32 q = 0; q < SAMPLES; q++) {
                    Float dx = (p + _random.uniform1D()) / SAMPLES;
                    Float dy = (q + _random.uniform1D()) / SAMPLES;
                    
                    Float lx = (p + _random.uniform1D()) / SAMPLES;
                    Float ly = (q + _random.uniform1D()) / SAMPLES;

                    // Build ray from camera
                    //Ray ray = camera.getPrimaryRay(pixel.x + dx, pixel.y + dy);
                    Point2 rand = Point2(lx, ly);
                    Ray ray = camera.getPrimaryRayDOF(pixel.x + dx, pixel.y + dy, rand);

                    color += traceRay(ray, 1.0, 1, pixel);
                }
            }
            color /= (SAMPLES * SAMPLES);*/


            //Color color = traceRay(ray, 1.0, 1, pixel);

            // Record sample on camera's film
            //camera.film().addColorSample(pixel.x, pixel.y, color);
        }
    }
}

Color WhittedRayTracer::estimateDirect(const SurfaceEvent& evt) const {
    Color direct = Color::BLACK;

    // Sample all lights
    for (Light const* light : _scene->getLights()) {
        Color  contrib = Color::BLACK;
        uint32 nSamples = light->numSamples();

        if (light->isDelta()) {
            Point2 bsdfRand  = Point2(_random.uniform1D(), _random.uniform1D());
            Point2 lightRand = Point2(_random.uniform1D(), _random.uniform1D());

            direct += sampleLight(*light, evt, lightRand, bsdfRand);
        } else {
            std::vector<Point2> bsdfVec;
            std::vector<Point2> lightVec;

            jittered2DArray(_random, nSamples, bsdfVec);
            jittered2DArray(_random, nSamples, lightVec);

            for (uint32 i = 0; i < nSamples; ++i)
                contrib += sampleLight(*light, evt, lightVec[i], bsdfVec[i]);

            direct += contrib / nSamples;
        }

    }

    return direct;
}

// Whitted algorithm
Color WhittedRayTracer::traceRay(const Ray& ray, Float ior, uint32 depth, const Vec2ui& pixel) const {
    Color Li = Color::BLACK;

    // Calculate intersection with scene
    SurfaceEvent event;
    bool intersect = _scene->intersectRay(ray, &event);

    if (!intersect) {
        if (depth == 1)
            return _scene->getBackgroundColor();

        return Color::BLACK;
    }

    const BSDF* bsdf = event.obj->bsdf();
    BSDFType surfType = bsdf->type();

    // Estimate direct lighting, except on specular surfaces
    if (!hasType(surfType, SPECULAR))
        Li += estimateDirect(event);

    if (depth >= _maxDepth)
        return Li;

    // Specular reflection
    if (hasType(bsdf->type(), REFLECTION)) {
        Normal n = event.sFrame.normal();
        Vec3 wi = Frame::reflect(event.wo, n);

        Ray reflect = event.spawnRay(event.toWorld(wi));

        Li += traceRay(reflect, 1.0, depth + 1);
    }

    // Specular transmission
    if (hasType(bsdf->type(), REFRACTION)) {
        Normal n = event.sFrame.normal();

        Float eta = bsdf->eta();
        if (Frame::cosTheta(event.wo) < 0)
            eta = 1.0 / eta;

        Vec3 wi = Frame::refract(event.wo, n, eta);

        Ray refract = event.spawnRay(event.toWorld(wi));

        Li += traceRay(refract, 1.0, depth + 1);
    }

    return Li;
}