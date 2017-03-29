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

            // Build ray from camera
            //Ray ray = camera.getPrimaryRay(pixel);

            // Get color from ray
            const uint32 SAMPLES = 8;
            Color3 color(0);
            for (uint32 p = 0; p < SAMPLES; p++) {
                for (uint32 q = 0; q < SAMPLES; q++) {
                    Float dx = (p + _random.uniformFloat()) / SAMPLES;
                    Float dy = (q + _random.uniformFloat()) / SAMPLES;

                    // Build ray from camera
                    Ray ray = camera.getPrimaryRay(pixel.x + dx, pixel.y + dy);

                    color += traceRay(ray, 1.0, 1, pixel);
                }
            }
            color /= (SAMPLES * SAMPLES);


            //Color3 color = traceRay(ray, 1.0, 1, pixel);

            // Record sample on camera's film
            camera.film().addColorSample(pixel.x, pixel.y, color);
        }
    }
}

// Whitted algorithm
Color3 WhittedRayTracer::traceRay(const Ray& ray, Float ior, uint32 depth, const Vec2ui& pixel) const {
    // Default color
    Color3 color = _scene->getBackgroundColor();

    // Calculate intersection with scene
    SurfaceEvent event;
    if (_scene->intersectRay(ray, &event)) {
        color = Vec3(0);
        const Material mtl = event.obj()->getMaterial();
        const std::vector<Light const*>& lights = _scene->getLights();

        // -> Direct Illumination
        // Calculate each light's contribution
        for (Light const* light : lights) {
            const Point3& lightPos = light->pos();
            const Vec3 lightDir = normalize(lightPos - event.point());
            if (dot(event.normal(), lightDir) > 0) {
                // Trace shadow ray from point to light position
                Ray shadowRay = event.spawnRay(lightPos);

                // If it is not occluded, calculate radiance at that point
                if (!_scene->isOccluded(shadowRay))
                    color += mtl.calcRadiance(light, event);
            }
        }

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