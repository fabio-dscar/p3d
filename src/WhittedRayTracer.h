#pragma once

#include <functional>

#include <MathDefs.h>
#include <Ray.h>
#include <Scene.h>
#include <Camera.h>

namespace Photon {

    class WhittedRayTracer {
    public:
        WhittedRayTracer() { }

        void initialize() {

        }

        void startRender() {
            using namespace std::placeholders;
            _group = ThreadUtils::pool->addTask(
                std::bind(&WhittedRayTracer::renderTile, this, *_scene, _3, _1),
                _tiles.size(),
                [&]() { }
            );
        }

        void renderTile(const Scene& scene, uint32 id, uint32 tileId) {
            const ImageTile& tile = _tiles[tileId];

            const Camera& camera = scene.getCamera();
            for (uint32 y = 0; y < tile.h; ++y) {
                for (uint32 x = 0; x < tile.w; ++x) {
                    // Build ray from camera
                    Ray ray = camera.getPrimaryRay(x, y);

                    // Get color from ray
                    Color3 color = traceRay(scene, ray, 1.0f, 1);

                    // Record sample on camera's film
                    camera.film().addColorSample(pixel, color);
                }
            }
        }

        // Whitted algorithm
        Color3 traceRay(const Scene& scene, Ray& ray, float ior, unsigned int depth) const {
            // Default color
            Color3 color = scene.getBackgroundColor();

            // Calculate intersection with scene
            SurfaceEvent event;
            if (scene.intersectRay(ray, &event)) {
                color = Vec3(0.0f);
                const Material mtl = event.obj()->getMaterial();
                const std::vector<std::shared_ptr<Light>>& lights = scene.getLights();

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

            return color;
        }

    private:
        uint16 _maxDepth;
        Scene* _scene;
    };
    
}