#include <WhittedRayTracer.h>

#include <Camera.h>
#include <Ray.h>
#include <Scene.h>

using namespace Photon;
using namespace Photon::Threading;
using namespace std::placeholders;

void WhittedRayTracer::initialize() {
    const Camera& camera = _scene->getCamera();
    uint32 width = camera.getWidth();
    uint32 height = camera.getHeight();

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
    // Initialize threading
    initThreads(getNumberOfProcessors());

    // Add task for drawing tiles in parallel
    _renderTask = Threading::Workers->pushTask(
        std::bind(&WhittedRayTracer::renderTile, this, std::ref(*_scene), _2, _1),
        _tiles.size(),
        endCallback
    );
}

void WhittedRayTracer::renderTile(const Scene& scene, uint32 tId, uint32 tileId) const {
    const ImageTile& tile = _tiles[tileId];

    const Camera& camera = scene.getCamera();
    for (uint32 y = 0; y < tile.h; ++y) {
        for (uint32 x = 0; x < tile.w; ++x) {
            Vec2u pixel(x + tile.x, y + tile.y);

            // Build ray from camera
            Ray ray = camera.getPrimaryRay(pixel);

            // Get color from ray
            Color3 color = traceRay(scene, ray, 1.0f, 1, pixel);

            // Record sample on camera's film
            camera.film().addColorSample(pixel.x, pixel.y, color);
        }
    }
}

// Whitted algorithm
Color3 WhittedRayTracer::traceRay(const Scene& scene, const Ray& ray, float ior, unsigned int depth, const Vec2u& pixel) const {
    // Default color
    Color3 color = scene.getBackgroundColor();

    // Calculate intersection with scene
    SurfaceEvent event;
    if (scene.intersectRay(ray, &event)) {
        color = Vec3(0.0f);
        const Material mtl = event.obj()->getMaterial();
        const std::vector<std::shared_ptr<Light>>& lights = scene.getLights();

        if (ray.isPrimary()) {
            scene.getCamera().film().addNormalSample(pixel.x, pixel.y, event.normal());
            scene.getCamera().film().addDepthSample(pixel.x, pixel.y, glm::length(ray.origin() - ray.hitPoint()));
        }

        // -> Direct Illumination
        // Calculate each light's contribution
        for (const std::shared_ptr<Light> light : lights) {
            const Vec3& lightPos = light->getPosition();
            const Vec3& lightDir = glm::normalize(lightPos - event.point());
            if (glm::dot(event.normal(), lightDir) > 0) {
                // Trace shadow ray from point to light position
                Ray shadowRay = Ray(event.point(), lightDir, lightPos);

                // If it is not occluded, calculate radiance at that point
                if (!scene.isOccluded(shadowRay))
                    color += mtl.calcRadiance(light, event);
            }
        }

        if (depth >= _maxDepth)
            return color;

        // -> Indirect Illumination
        if (mtl.isTransmissive()) {
            // Transmitted direction
            // If there is total internal reflection, the returned ray is the reflection ray
            Ray refracted = ray.refract(event, ior);

            // Transmitted ray contribution
            color += mtl.getTransmit() * traceRay(scene, refracted, ior, depth + 1);
        }

        if (mtl.isReflector()) {
            // Reflected ray
            Ray reflected = ray.reflect(event);

            // Reflected ray contribution
            color += mtl.getSpec() * traceRay(scene, reflected, ior, depth + 1);
        }
    }

    if (!event.hit() && ray.isPrimary())
        scene.getCamera().film().addNormalSample(pixel.x, pixel.y, -ray.dir());

    return color;
}

bool WhittedRayTracer::hasCompleted() {
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