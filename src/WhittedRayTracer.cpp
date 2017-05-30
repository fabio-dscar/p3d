#include <WhittedRayTracer.h>

#include <Camera.h>
#include <Ray.h>
#include <Scene.h>

#include <AreaLight.h>
#include <BSDF.h>
#include <Records.h>
#include <Shape.h>

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

                const Ray ray = camera.primaryRay(pixel, sampler);
                Color Li = traceRay(ray, 1, sampler, pixel);

                // Record sample on camera's film
                camera.film().addColorSample(pixel.x, pixel.y, Li);

                color += Li;
            }

            // Use a box filter for the preview
            color /= sampler.spp();
            camera.film().addPreviewSample(pixel.x, pixel.y, color);
        }
    }
}

// Whitted algorithm
Color WhittedRayTracer::traceRay(const Ray& ray, uint32 depth, Sampler& sampler, const Point2ui& pixel) const {
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

    // Estimate direct lighting
    Li += estimateDirectAll(event, sampler);

    if (depth >= _maxDepth)
        return Li;

    // Store primary ray's scene features 
    if (depth == 1) {
        FeaturesRecord feat;
        feat.dist   = (ray.origin() - event.point).length();
        feat.raster = Point2(pixel.x, pixel.y);
        feat.normal = event.sFrame.normal();

        _scene->getCamera().film().addFeatureSample(feat);
    }

    // Specular reflection
    if (hasType(bsdf->type(), REFLECTION)) {
        Normal n = event.sFrame.normal();
        Vec3 wi = Frame::reflect(event.wo, n);

        // Evaluate BSDF for wi and wo
        BSDFSample bs(event, wi, RADIANCE);
        Color f = bsdf->eval(bs);
        
        if (f.isBlack())
            return Li;

        Ray reflect = event.spawnRay(event.toWorld(wi));

        Li += f * absDot(n, wi) * traceRay(reflect, depth + 1, sampler);
    }

    // Specular transmission
    if (hasType(bsdf->type(), REFRACTION)) {
        Normal n = event.sFrame.normal();

        Float eta = bsdf->eta();
        if (Frame::cosTheta(event.wo) < 0)
            eta = 1.0 / eta;

        Vec3 wi = Frame::refract(event.wo, n, eta);

        // Evaluate BSDF for wi and wo
        BSDFSample bs(event, wi, RADIANCE);
        Color f = bsdf->eval(bs);

        if (f.isBlack())
            return Li;

        Ray refract = event.spawnRay(event.toWorld(wi));

        Li += f * absDot(n, wi) * traceRay(refract, depth + 1, sampler);
    }

    return Li;
}