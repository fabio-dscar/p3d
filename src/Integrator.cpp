#include <Integrator.h>

#include <Scene.h>
#include <Camera.h>
#include <Ray.h>
#include <Light.h>
#include <AreaLight.h>

using namespace Photon;

void Integrator::initialize() {
    const Camera& camera = _scene->getCamera();
    uint32 width  = camera.width();
    uint32 height = camera.height();

    // Divide in tiles
    for (uint32 y = 0; y < height; y += _tileSize) {
        for (uint32 x = 0; x < width; x += _tileSize) {
            Vec2ui tileSize;
            tileSize.x = std::min(_tileSize, width - x);
            tileSize.y = std::min(_tileSize, height - y);

            _tiles.emplace_back(*_sampler.get(), 
                                Point2ui(x, y),
                                tileSize
            );
        }
    }    
}

bool Integrator::hasCompleted() const {
    return _renderTask->isDone();
}

void Integrator::waitForCompletion() {
    if (_renderTask) {
        _renderTask->wait();
        _renderTask.reset(); // Free pointer
    }
}

void Integrator::cleanup() {
    _renderTask.reset(); // Free pointer
    _tiles.clear();
    _tiles.shrink_to_fit();
}

Color Integrator::sampleBsdf(const SurfaceEvent& evt, const Point2& randBsdf) const {
    Color Ldir = Color::BLACK;
    Float pdfLight;
    Color bsdfF;

    const BSDF& bsdf = *evt.obj->bsdf();

    // Sample and eval BSDF
    BSDFSample bsdfSample(evt);
    bsdfF = bsdf.sample(randBsdf, &bsdfSample);
    bsdfF *= Frame::absCosTheta(bsdfSample.wi);

    if (bsdfSample.pdf > 0 && !bsdfF.isBlack() &&
        dot(evt.normal, evt.toWorld(bsdfSample.wi)) * Frame::cosTheta(bsdfSample.wi) > 0) {

        // World space bsdf sampled direction
        Vec3 wi = evt.toWorld(bsdfSample.wi);

        // Evaluate light emission
        Color Li = Color::BLACK;
        Ray ray = evt.spawnRay(wi);

        SurfaceEvent lightIt;
        const Light* light = nullptr;
        if (_scene->intersectRay(ray, &lightIt)) {
            if (!lightIt.obj->isLight())
                return Color::BLACK;

            Li = lightIt.emission(-wi);
            light = lightIt.obj->areaLight();
        } else {
            // Evaluate infinite light (if that is the case)

            //Li = light.evalInfinite
        }

        if (light == nullptr)
            return Color::BLACK;

        // Compute Le contributions from sampled wi
    
        // Evaluate light pdf for BSDF sample
        // If BSDF is specular, it has a delta distribution, so avoid using MIS
		Float misWeight = 1.0;
		DirectSample sample(evt, lightIt);
        pdfLight = light->pdfDirect(sample);
        if (pdfLight == 0)
            return Color::BLACK;

        misWeight = powerHeuristicBetaTwo(bsdfSample.pdf, pdfLight, 1, 1);
 
        if (!Li.isBlack())
            Ldir += bsdfF * Li * misWeight / bsdfSample.pdf;
    }

    return Ldir;
}

Color Integrator::sampleLight(const Light& light, const SurfaceEvent& evt, const Point2& randLight, const Point2& randBsdf) const {
    const BSDF* bsdf = evt.obj->bsdf();
    Color Ldir = Color::BLACK;

    /* -----------------------------------------------------------------------------------
            Sample Light with MIS
    --------------------------------------------------------------------------------------*/
    DirectSample dirSample(evt);
    Color Li = light.sampleDirect(randLight, &dirSample);

    if (dirSample.pdf > 0 && !Li.isBlack() && !bsdf->isType(BSDFType::SPECULAR)) {
        Color bsdfF   = Color::BLACK;
        Float bsdfPdf = 0;

        // Trace shadow ray from point in direction wi
        Ray shadowRay = evt.spawnRay(dirSample.wi, dirSample.dist);
        if (!_scene->isOccluded(shadowRay)) {

            // Evaluate BSDF for direct sample
            BSDFSample bsdfSample(dirSample);
            bsdfF   = bsdf->eval(bsdfSample);
            bsdfPdf = bsdf->evalPdf(bsdfSample);

            // If it has contribution, use MIS to combine sample strategies
            // Also check geometry normal orientation to avoid light leaks
            if (!bsdfF.isBlack() && dot(evt.normal, dirSample.wi) * Frame::cosTheta(bsdfSample.wi) > 0) {
                Color contrib = bsdfF * Li; // / dirSample.pdf;
                if (!light.isDelta())
                    contrib *= powerHeuristicBetaTwo(dirSample.pdf, bsdfPdf, 1, 1);

                Ldir += contrib;
            }
        }
    }

    /* -----------------------------------------------------------------------------------
            Sample BSDF with MIS
    --------------------------------------------------------------------------------------*/
    if (!light.isDelta()) {
        Float pdfLight;
        Color bsdfF;

        // Sample and eval BSDF
        BSDFSample bsdfSample(evt);
        bsdfF  = bsdf->sample(randBsdf, &bsdfSample);
        bsdfF *= Frame::absCosTheta(bsdfSample.wi);

        if (bsdfSample.pdf > 0 && !bsdfF.isBlack() &&
            dot(evt.normal, evt.toWorld(bsdfSample.wi)) * Frame::cosTheta(bsdfSample.wi) > 0) {

            // World space bsdf sampled direction
            Vec3 bsdfWi = evt.toWorld(bsdfSample.wi);

            // Evaluate light emission
            Color Li = Color::BLACK;
            Ray ray  = evt.spawnRay(bsdfWi);

            // Check if it intersects the light being sampled
            SurfaceEvent lightIt;
            if (_scene->intersectRay(ray, &lightIt)) {
                //if (lightIt.obj->areaLight() == &light)
                if (lightIt.obj->isLight())
                    Li = lightIt.emission(-bsdfWi);
            } else {
                // Evaluate infinite light (if that is the case)
            }

            // Compute Le contributions from sampled wi
            if (!Li.isBlack()) {
                Float misWeight = 1.0;
                DirectSample sample(evt, lightIt);
                sample.wi = bsdfWi;

                // Evaluate light pdf for BSDF sample
                // If BSDF is specular, it has a delta distribution, so avoid using MIS
                if (!hasType(bsdfSample.type, BSDFType::SPECULAR)) {
                    pdfLight = light.pdfDirect(sample);
                    if (pdfLight == 0)
                        return Ldir;

                    misWeight = powerHeuristicBetaTwo(bsdfSample.pdf, pdfLight, 1, 1);
                }

                Ldir += bsdfF * Li * misWeight / bsdfSample.pdf;
            }
        }
    }

    return Ldir;
}

