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
            _random.jittered2DArray(SAMPLES, SAMPLES, pixelSamples, true);
            _random.jittered2DArray(SAMPLES, SAMPLES, lensSamples, true);

            // Compute ray tracing
            Color color(0);        
            for (uint32 i = 0; i < pixelSamples.size(); ++i) {
                Color lensEstimate(0);
                Point2 ps = pixelSamples[i];
                //Point2 ls = lensSamples[i];

                //Point2 rand = Point2(ls.x, ls.y);
                Point2 p = Point2(pixel.x + ps.x, pixel.y + ps.y);
                //Ray ray = camera.getPrimaryRayDOF(p, rand);

                Ray ray = camera.getPrimaryRay(pixel.x + ps.x, pixel.y + ps.y);

                color += traceRay(ray, 1.0, 1, pixel);
            }

            // Filter result
            color /= (SAMPLES * SAMPLES);

            // Record sample on camera's film
            camera.film().addColorSample(pixel.x, pixel.y, color);

            /*Color color(0);
            for (uint32 p = 0; p < SAMPLES; p++) {
                for (uint32 q = 0; q < SAMPLES; q++) {
                    Float dx = (p + _random.uniformFloat()) / SAMPLES;
                    Float dy = (q + _random.uniformFloat()) / SAMPLES;
                    
                    Float lx = (p + _random.uniformFloat()) / SAMPLES;
                    Float ly = (q + _random.uniformFloat()) / SAMPLES;

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
    const std::vector<Light const*>& lights = _scene->getLights();
    const Material mtl = evt.obj()->getMaterial();
    Color direct = Color::BLACK;

    // Sample all lights
    for (Light const* light : lights) {
        Color  contrib = Color::BLACK;
        uint32 nSamples = light->numSamples();

        if (light->isDelta()) {
            Point2 bsdfRand  = Point2(_random.uniformFloat(), _random.uniformFloat());
            Point2 lightRand = Point2(_random.uniformFloat(), _random.uniformFloat());

            direct += sampleLight(*light, evt, lightRand, bsdfRand);
        } else {
            std::vector<Point2> bsdfVec;
            std::vector<Point2> lightVec;

            bsdfVec.reserve(nSamples * nSamples);
            lightVec.reserve(nSamples * nSamples);

            _random.jittered2DArray(nSamples, nSamples, bsdfVec, true);
            _random.jittered2DArray(nSamples, nSamples, lightVec, true);

            for (uint32 i = 0; i < (nSamples * nSamples); ++i)
                contrib += sampleLight(*light, evt, lightVec[i], bsdfVec[i]);

            direct += (contrib / lightVec.size());
        }

    }

    return direct;
}

// Whitted algorithm
Color WhittedRayTracer::traceRay(const Ray& ray, Float ior, uint32 depth, const Vec2ui& pixel) const {
    // Default color
    Color color(0); //= _scene->getBackgroundColor();

    if (depth == 1)
        color = _scene->getBackgroundColor();

    //if (pixel == Vec2ui(60, 239))
    //    std::cout << "Stop!";

    // Calculate intersection with scene
    SurfaceEvent event;
    if (_scene->intersectRay(ray, &event)) {
        // Get self emission from intersected object
        color = event.Le(event.toWorld(event.wo()));

        if (event.obj()->isLight())
            return color;

        const Material mtl = event.obj()->getMaterial();

        // -> Direct Illumination
        // Calculate direct lighting
        color += estimateDirect(event);

        if (depth >= _maxDepth)
            return color;

        // -> Indirect Illumination
        /*const BSDF* bsdf = event.obj()->bsdf();

        BSDFSample sample(event);
        Point2 rand = Point2(_random.uniformFloat(), _random.uniformFloat());
        Color f = bsdf->sample(rand, &sample);

        if (sample.pdf == 0 || f.isBlack()) 
            return color;

        Ray r  = Ray(event.point(), event.toWorld(sample.wi));
        color += f * Frame::absCosTheta(sample.wi) * traceRay(r, ior, depth + 1) / sample.pdf;*/

        if (mtl.isReflector()) {
            // Reflected ray
            Ray reflected = ray.reflect(event);

            //color += specularReflect(event, depth);
            
            // Reflected ray contribution
            color += mtl.getSpec() * traceRay(reflected, ior, depth + 1);
        }

        if (mtl.isTransmissive()) {
            // Transmitted direction
            // If there is total internal reflection, the returned ray is the reflection ray
            Ray refracted = ray.refract(event, ior);

            // Transmitted ray contribution
            color += mtl.getTransmit() * traceRay(refracted, ior, depth + 1);
        }
    }

    return color;
}