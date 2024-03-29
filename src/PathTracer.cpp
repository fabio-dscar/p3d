#include <PathTracer.h>

#include <PointLight.h>
#include <AreaLight.h>

#include <Random.h>

#ifdef PHOTON_MSVC
//#pragma warning(disable : 4838)
#endif

using namespace Photon;
using namespace Photon::Threading;
using namespace std::placeholders;

void PathTracer::initialize() {
    
    if (_scene->lightStrategy() == ALL_LIGHTS) {
        std::vector<Light*> lights = _scene->getLights();

        for (uint32 spp = 0; spp < _spp; ++spp) {
            for (const Light* l : lights) {
                uint32 numSamples = l->numSamples();
                _sampler->allocArray2D(numSamples); // Light
                _sampler->allocArray2D(numSamples); // BSDF
            }
        }
    }

    Integrator::initialize();
}

void PathTracer::startRender(EndCallback endCallback) {
    auto taskFunc = std::bind(&PathTracer::renderTile, this, _2, _1);
    if (_useAdaptive)
        taskFunc = std::bind(&PathTracer::renderTileAdaptive, this, _2, _1);

    // Add task for drawing tiles in parallel
    _renderTask = Threading::Workers->pushTask(
        taskFunc,
        uint32(_tiles.size()),
        endCallback
    );
}

bool PathTracer::checkAdaptiveThreshold(const Color* samples, uint32 num) const {
    for (uint32 i = num-1; i >= 1; --i) {
        for (int32 it = i - 1; it >= 0; --it) {
            Color diff = samples[i] - samples[it];
            Float max = std::max(abs(diff.r), std::max(abs(diff.g), abs(diff.b)));
            //return false;
            if (max > 0.05) // Threshold check
                return true;
        }
    }
        
    return false;
}

Color PathTracer::subdivide(Sampler& sampler, const Point2& min, const Point2& max, std::vector<Color>& table, const Point2ui& pixel, Float weight, uint32* nSamples) const {
    const Camera& camera = _scene->getCamera();
    Color res = Color::BLACK;

    const Point2 corners[4] = {
        Point2(min.x, max.y), // Top left
        max,                  // Top right
        min,                  // Bottom left
        Point2(max.x, min.y)  // Bottom right
    };

    const Float halfX = (max.x + min.x) / 2.0;
    const Float halfY = (max.y + min.y) / 2.0;

    // The 5 new samples
    const Point2 samples[5] = {
        Point2(halfX, max.y), // Half top
        Point2(min.x, halfY), // Half left
        Point2(halfX, halfY), // Center
        Point2(max.x, halfY), // Half right
        Point2(halfX, min.y)  // Half bottom
    };

    // Index of adjacent samples for each corner
    const uint32 adjIdx[4][3] = {
        { 0, 1, 2 },    // For top left corner
        { 0, 2, 3 },    // For top right corner
        { 1, 2, 4 },    // For bottom left corner
        { 2, 3, 4 }     // For botoom left corner
    };
  
    Color Li[5];

    const uint32 w = _adaptWidth;
    const uint32 h = _adaptHeight;

    // Trace paths through the new 5 sample points
    for (uint32 i = 0; i < 5; i++) {
        Point2 p = Point2(samples[i].x + pixel.x,
                          samples[i].y + pixel.y);
        uint32 tblIdx = (samples[i].x * (w - 1)) +
                        w * ((h - 1) * samples[i].y);

        // Compute integrand evaluation
        Ray ray = camera.primaryRay(p, samples[i]);
        Li[i] = tracePath(ray, sampler, pixel);
        
        // Store in sample table
        table[tblIdx] = Li[i];
    }

    // Iterate the 4 corners
    for (uint32 i = 0; i < 4; i++) {
        // Map corner to 1D array
        uint32 tableIdx = (corners[i].x * (w - 1)) +
                          w * ((h - 1) * corners[i].y);

        uint32 cornerIdx[4] = {
            (corners[i].x * (w - 1)) + w * ((h - 1) * corners[i].y),
            (samples[adjIdx[i][0]].x * (w - 1)) + w * ((h - 1)  * samples[adjIdx[i][0]].y),
            (samples[adjIdx[i][1]].x * (w - 1)) + w * ((h - 1)  * samples[adjIdx[i][1]].y),
            (samples[adjIdx[i][2]].x * (w - 1)) + w * ((h - 1)  * samples[adjIdx[i][2]].y)
        };

        Color colors[4];
        for (uint32 idx = 0; idx < 4; ++idx)
            colors[idx] = table[cornerIdx[idx]];

        bool recurse = checkAdaptiveThreshold(colors, 4);
        
        // If there is no need to subdivide
        if (!recurse || weight <= 1.0 / (_adaptWidth * _adaptHeight)) {
            // Compute average to return
            Color avg = Color::BLACK;
            for (uint32 s = 0; s < 4; ++s)
                avg += colors[s];
            avg *= weight;  // Weight according to the area they represent

            res += avg / 4.0; // Add average to current estimate
        } else {
            // Recurse 
            Point2 min = corners[i];
            Point2 max = corners[i];

            for (uint32 s = 0; s < 3; ++s) {
                min = Math::min(min, samples[adjIdx[i][s]]);
                max = Math::max(max, samples[adjIdx[i][s]]);
            }
            
            *nSamples += 5;
            res += subdivide(sampler, min, max, table, pixel, weight / 4.0, nSamples);
        }
    }

    return res;
}

void PathTracer::renderTileAdaptive(uint32 tId, uint32 tileId) const {
    const ImageTile& tile = _tiles[tileId];
    Sampler& sampler = *tile.samp.get();

    const uint32 w = _adaptWidth;
    const uint32 h = _adaptHeight;

    const Point2 pts[4] = {
        Point2(0.0, 1.0),
        Point2(1.0, 1.0),
        Point2(0.0, 0.0),
        Point2(1.0, 0.0)
    };

    // Maps of pts to 1D array
    const uint32 tblIdx[4] = {
        w * (h - 1), 
        (w - 1) + w * (h - 1),
        0,
        (w - 1)
    };

    // Initialize 1D array for samples
    std::vector<Color> sampleTable(w * h); 

    const Camera& camera = _scene->getCamera();
    for (uint32 y = 0; y < tile.h; ++y) {
        for (uint32 x = 0; x < tile.w; ++x) {
            Point2ui pixel(x + tile.x, y + tile.y);
    
            sampler.start(pixel);

            Color samples[4];
            Color sum = Color::BLACK;
            for (uint32 i = 0; i < 4; i++) {
                Point2 p = Point2(pixel.x + pts[i].x, pixel.y + pts[i].y);
                Ray ray = camera.primaryRay(p, pts[i]);
                samples[i] = tracePath(ray, sampler, pixel);
                sum += samples[i];
                sampleTable[tblIdx[i]] = samples[i];
            }
            
            uint32 nSamples = 4;
            Color Li = Color::BLACK;
            if (!checkAdaptiveThreshold(&samples[0], 4)) {
                Li = sum / 4.0;  // Just compute the average
            } else {
                nSamples += 5; // Compute 5 new samples
                Li = subdivide(sampler, pts[2], pts[1], sampleTable, pixel, 1.0 / 4.0, &nSamples);
            }

            camera.film().addColorSample(pixel.x, pixel.y, Li, nSamples);
            camera.film().addPreviewSample(pixel.x, pixel.y, Li);
        }
    }

}

// This is called by different threads
void PathTracer::renderTile(uint32 tId, uint32 tileId) const {
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

                //pixel = Point2ui(253, 348);

                const Ray ray = camera.primaryRay(pixel, sampler);
                Color Li = tracePath(ray, sampler, pixel);

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

#define DEBUG(str) std::cout << str << std::endl;

Color PathTracer::tracePath(const Ray& ray, Sampler& sampler, const Point2ui& pixel) const {
    Color Li = Color::BLACK;
    Ray   subPath = ray;           // Current sub-path

    Color beta      = Color(1.0);  // Path throughput
    Float refrScale = 1.0;         // Refraction scaling

    const BSDF* bsdf = nullptr;

    uint32 depth = 1;
    while (depth <= _maxDepth) {
        SurfaceEvent event = SurfaceEvent();
        bool intersect = _scene->intersectRay(subPath, &event);

        if (!intersect) {
            // If this was a primary ray or
            // If we just left a specular material, return the background color
            // (TODO: infinite lights instead of background color)
            if (subPath.isPrimary() || (bsdf && bsdf->isType(BSDFType::SPECULAR)))
                Li += beta * _scene->getBackgroundColor();

            break;  // Leave loop
        } 
        
        // Check if intersected object is self emissive and add its contribution
        if (subPath.isPrimary()) { // || (bsdf && bsdf->isType(BSDFType::SPECULAR))) { // || bsdf->isType(BSDFType::GLOSSY)))) {
            if (event.obj->isLight())
                Li += beta * event.emission(-subPath.dir());
        }

        // Fetch intersection's BSDF
        bsdf = event.obj->bsdf();
        if (!bsdf || bsdf->isType(BSDFType::NONE))
            break;  // Leave if null bsdf (e.g. light sources)

        // Avoid light leaks
        if (dot(event.normal, -subPath.dir()) * Frame::cosTheta(event.wo) <= 0)
            break;

        /* -----------------------------------------------------------------------------------
                Direct Illumination
        --------------------------------------------------------------------------------------*/
        DirectIllumStats dlStats;
        Li += beta * estimateDirect(event, sampler, &dlStats);

        /* -----------------------------------------------------------------------------------
                Indirect Illumination
        --------------------------------------------------------------------------------------*/
        // Sample a direction from the BSDF
        BSDFSample sample(event);
        Color f = bsdf->sample(sampler.next2D(), &sample);

        // Leave if no contribution from sampled direction
        if (sample.pdf == 0 || f.isBlack())
            break;

        // If we just sampled refraction, keep track of radiance scaling
        if (hasType(sample.type, BSDFType(BSDFType::REFRACTION)))
            refrScale *= sample.eta * sample.eta;

        // Update the throughput
        beta *= f * Frame::absCosTheta(sample.wi) / sample.pdf;

        // Possibly end path with russian roulette
        Float rr = (refrScale * beta).max();
        if (depth > 4 && rr < 0.8) {
            Float q = Math::max(0.01, 1.0 - rr);
            if (sampler.next1D() < q)
                break;  // Stop path
            
            beta /= (1 - q);
        }

        // Store primary ray's scene features
        if (subPath.isPrimary()) {
            FeaturesRecord feat;
            feat.dist   = (ray.origin() - event.point).length();
            feat.raster = Point2(pixel.x, pixel.y);
            feat.normal = event.sFrame.normal();

            if (dlStats.numRays > 0)
                feat.vis = dlStats.numUnoccluded / dlStats.numRays;

            _scene->getCamera().film().addFeatureSample(feat);
        }

        // Spawn a new ray in the sampled direction
        subPath = event.spawnRay(event.toWorld(sample.wi));

        depth++;

#ifdef PHOTON_DEBUG
        if (std::isinf(Li.r) || std::isnan(Li.r) ||
            std::isinf(Li.g) || std::isnan(Li.g) ||
            std::isinf(Li.b) || std::isnan(Li.b)) {
            DEBUG("Radiance not a number/infinite!!");
        }

        if (Li.r < 0 || Li.g < 0 || Li.b < 0)
            DEBUG("Radiance below zero!!");
#endif

    }

    return Li;
}

