#include <PathTracer.h>

using namespace Photon;
using namespace Photon::Threading;
using namespace std::placeholders;

void PathTracer::startRender(EndCallback endCallback) {
    // Add task for drawing tiles in parallel
    _renderTask = Threading::Workers->pushTask(
        std::bind(&PathTracer::renderTile, this, _2, _1),
        uint32(_tiles.size()),
        endCallback
    );
}

// This is called by different threads
void PathTracer::renderTile(uint32 tId, uint32 tileId) const {
    const ImageTile& tile = _tiles[tileId];

    const Camera& camera = _scene->getCamera();
    for (uint32 y = 0; y < tile.h; ++y) {
        for (uint32 x = 0; x < tile.w; ++x) {
            Vec2ui pixel(x + tile.x, y + tile.y);

            // Get color from ray
            const uint32 SAMPLES = 4; // 16
            const uint32 LENS_SAMPLES = 4;

            // Get jittered samples
            std::vector<Point2> pixelSamples;
            std::vector<Point2> lensSamples;
            _random.jittered2DArray(SAMPLES, SAMPLES, pixelSamples, true);
            //_random.jittered2DArray(SAMPLES, SAMPLES, lensSamples, true);

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

                color += tracePath(ray, 1.0, 1, pixel);
            }

            // Filter result
            color /= (SAMPLES * SAMPLES);

            // Record sample on camera's film
            camera.film().addColorSample(pixel.x, pixel.y, color);
        }
    }
}

Color PathTracer::estimateDirect(const SurfaceEvent& evt) const {
    const std::vector<Light const*>& lights = _scene->getLights();
    const Material mtl = evt.obj()->getMaterial();
    Color direct = Color::BLACK;

    // Sample all lights
    for (Light const* light : lights) {
        Color  contrib = Color::BLACK;
        uint32 nSamples = light->numSamples();

        if (light->isDelta()) {
            Point2 bsdfRand = Point2(_random.uniformFloat(), _random.uniformFloat());
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
Color PathTracer::tracePath(const Ray& ray, Float ior, uint32 depth, const Vec2ui& pixel) const {
    // Default color
    Color Li = Color::BLACK; //= _scene->getBackgroundColor();

    Color beta(1.0);
    Float eta = 1.0;

    Ray r = ray;
    const BSDF* bsdf = nullptr;
    
    while (depth <= _maxDepth) {
        SurfaceEvent event = SurfaceEvent();
        bool intersect = _scene->intersectRay(r, &event);

        if (!intersect) {
            // If we just left a specular material, return the background color
            // Or if this is a primary ray
            if (r.isPrimary() || (bsdf && bsdf->isType(BSDFType::SPECULAR)))
                Li += beta * _scene->getBackgroundColor();

            break;  // Leave loop
        }

        // Fetch intersection's BSDF
        bsdf = event.obj()->bsdf();

        // -> Direct Illumination
        // Only perform direct illumination if not on specular surface
        if (!bsdf->isType(BSDFType::SPECULAR))
            Li += beta * estimateDirect(event);

        // -> Indirect Illumination
        BSDFSample sample(event);
        Point2 rand = Point2(_random.uniformFloat(), _random.uniformFloat());
        //sample.type = BSDFType(sample.type ^ BSDFType::REFRACTION);
        Color f = bsdf->sample(rand, &sample);

        // Leave if no contribution from sampled direction
        if (sample.pdf == 0 || f.isBlack())
            break;

        // If we just sampled refraction, update the eta scale
        if (hasType(sample.type, BSDFType(BSDFType::SPECULAR | BSDFType::REFRACTION))) {
            if (dot(event.toWorld(event.wo()), event.normal()) > 0)
                eta *= eta;
            else
                eta *= eta;
        }

        if (r.isPrimary()) {
            const Camera& c = _scene->getCamera();
            //c.film().addNormalSample(pixel.x, pixel.y, normalize(event.wo()));
            
            //c.film().addNormalSample(pixel.x, pixel.y, normalize(abs(sample.wi)));
            c.film().addNormalSample(pixel.x, pixel.y, normalize(abs(event.toWorld(event.wo()))));
        }

        // Update the throughput
        beta *= f * Frame::absCosTheta(sample.wi); // / sample.pdf;

        // Set a new ray in the new direction
        r = Ray(event.point(), event.toWorld(sample.wi));     
        
        depth++;
    }

    return Li;
}

