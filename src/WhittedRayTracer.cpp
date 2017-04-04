#include <WhittedRayTracer.h>

#include <Camera.h>
#include <Ray.h>
#include <Scene.h>

using namespace Photon;
using namespace Photon::Threading;
using namespace std::placeholders;

void WhittedRayTracer::initialize() {
    const Camera& camera = _scene->getCamera();
    uint32 width = camera.width();
    uint32 height = camera.height();

    // Divide in tiles
    for (uint32 y = 0; y < height; y += TILE_SIZE) {
        for (uint32 x = 0; x < width; x += TILE_SIZE) {
            _tiles.emplace_back(x,
                                y,
                                std::min(TILE_SIZE, width - x),
                                std::min(TILE_SIZE, height - y)
            );
        }
    }
}

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
            const uint32 SAMPLES = 4; // 16

            // Get jittered samples
            std::vector<Point2> pixelSamples;
            std::vector<Point2> lensSamples;
            _random.jittered2DArray(SAMPLES, SAMPLES, pixelSamples, true);
            //_random.jittered2DArray(SAMPLES, SAMPLES, lensSamples, true);

            // Compute ray tracing
            Color3 color(0);
            for (uint32 i = 0; i < pixelSamples.size(); ++i) {
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

            /*Color3 color(0);
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


            //Color3 color = traceRay(ray, 1.0, 1, pixel);

            // Record sample on camera's film
            //camera.film().addColorSample(pixel.x, pixel.y, color);
        }
    }
}

struct DirectSample {
    Point3 pos;
    Float pdf;

    DirectSample(const Point3& pos, Float pdf) 
        : pos(pos), pdf(pdf) { };
};

#include <AreaLight.h>

Color3 WhittedRayTracer::estimateDirect(const SurfaceEvent& evt) const {
    const std::vector<Light const*>& lights = _scene->getLights();
    const Material mtl = evt.obj()->getMaterial();
    Color3 direct(0);

    const uint32 SAMPLES_PER_LIGHT = 4; // 256

    // Sample all lights
    for (Light const* light : lights) {
        Color3 contrib(0);
        Point3 pos; Float pdf;
        std::vector<DirectSample> samples;

        uint32 nSamples = SAMPLES_PER_LIGHT;
        std::vector<Point2> vec;

        // If light is a point, only sample once 
        if (light->isDelta()) {
            light->sampleLi(evt, Point2(_random.uniformFloat(), _random.uniformFloat()), &pos, &pdf);
            samples.emplace_back(pos, (Float)1.0);
        } else {
            vec.reserve(SAMPLES_PER_LIGHT * SAMPLES_PER_LIGHT);
            _random.jittered2DArray(SAMPLES_PER_LIGHT, SAMPLES_PER_LIGHT, vec, true);

            for (const Point2& sample : vec) {
                light->sampleLi(evt, sample, &pos, &pdf);
                samples.emplace_back(pos, (Float)1.0);
            }
        }

        // Fetch samples
        /*for (uint32 i = 0; i < nSamples; i++) {
            light->sampleLi(Point2(_random.uniformFloat(), _random.uniformFloat()), &pos, &pdf);
            samples.emplace_back(pos, pdf);
        }*/

        // Estimate integral with samples
        for (const DirectSample sample : samples) {
            Vec3 dir = normalize(sample.pos - evt.point());

            // Check orientation
            if (dot(evt.normal(), dir) > 0) {
                // Trace shadow ray from point to sampled position
                Ray shadowRay = evt.spawnRay(sample.pos);

                // If it is not occluded, calculate radiance
                if (!_scene->isOccluded(shadowRay)) {
                    Float distSqr = 1.0 / Math::distSqr(sample.pos, evt.point());
                    distSqr = 1;
                    contrib += distSqr * light->L(evt, dir) * mtl.calcRadiance(dir, evt) / sample.pdf;
                }
            }
        }

        // Average this light's contribution
        if (!light->isDelta())
            contrib /= vec.size();

        // Sum with the direct lighting estimation
        direct += contrib;

        samples.clear();
    }

    return direct;
}

// Whitted algorithm
Color3 WhittedRayTracer::traceRay(const Ray& ray, Float ior, uint32 depth, const Vec2ui& pixel) const {
    // Default color
    Color3 color = _scene->getBackgroundColor();

    //if (pixel == Vec2ui(60, 239))
    //    std::cout << "Stop!";

    // Calculate intersection with scene
    SurfaceEvent event;
    if (_scene->intersectRay(ray, &event)) {
        // Get self emission from intersected object
        color = event.Le(event.wo());

        const Material mtl = event.obj()->getMaterial();

        // -> Direct Illumination
        // Calculate direct lighting
        color += estimateDirect(event);

        if (depth >= _maxDepth)
            return color;

        if (mtl.isReflector()) {
            // Reflected ray
            Ray reflected = ray.reflect(event);

            // Reflected ray contribution
            color += mtl.getSpec() * traceRay(reflected, ior, depth + 1);
        }

        // -> Indirect Illumination
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

bool WhittedRayTracer::hasCompleted() const {
    return _renderTask->isDone();
}

void WhittedRayTracer::waitForCompletion() {
    if (_renderTask) {
        _renderTask->wait();
        _renderTask.reset(); // Free pointer
    }
}

void WhittedRayTracer::cleanup() {
    _renderTask.reset(); // Free pointer
    _tiles.clear();
    _tiles.shrink_to_fit();
}