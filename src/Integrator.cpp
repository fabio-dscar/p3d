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
            _tiles.emplace_back(x,
                                y,
                                std::min(_tileSize, width - x),
                                std::min(_tileSize, height - y)
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

Color Integrator::sampleLight(const Light& light, const SurfaceEvent& evt, const Point2& randLight, const Point2& randBsdf) const {
    const BSDF* bsdf = evt.obj()->bsdf();
    Color Ldir = Color::BLACK;

    /* -----------------------------------------------------------------------------------
            Sample Light with MIS
    --------------------------------------------------------------------------------------*/
    DirectSample dirSample(evt);
    Color Li = light.sampleDirect(randLight, &dirSample);

    if (dirSample.pdf > 0 && !Li.isBlack()) {
        Color bsdfF = 0;
        Float bsdfPdf = 0;

        // Trace shadow ray from point in direction wi
        Ray shadowRay = evt.spawnRay(dirSample.wi, dirSample.dist);
        if (_scene->isOccluded(shadowRay))
            return Color::BLACK;

        // Evaluate BSDF for direct sample
        BSDFSample bsdfSample(dirSample);
        bsdfF = bsdf->eval(bsdfSample);
        bsdfPdf = bsdf->evalPdf(bsdfSample);

        // If it has contribution, use MIS to combine sample strategies
        // Also check geometry normal orientation to avoid light leaks
        if (!bsdfF.isBlack() && dot(evt.normal(), dirSample.wi) * Frame::cosTheta(bsdfSample.wi) > 0) {
            if (light.isDelta())
                Ldir += bsdfF * Li;
            else
                Ldir += bsdfF * Li * powerHeuristicBetaTwo(dirSample.pdf, bsdfPdf, 1, 1);
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
        bsdfF = bsdf->sample(randBsdf, &bsdfSample);
        bsdfF *= Frame::absCosTheta(bsdfSample.wi); //absDot(evt.sFrame().normal(), bsdfSample.wi);

        if (bsdfSample.pdf > 0 && !bsdfF.isBlack() &&
            dot(evt.normal(), evt.toWorld(bsdfSample.wi)) * Frame::cosTheta(bsdfSample.wi) > 0) {

            // Compute Le contributions from sampled wi
            Float misWeight = 1;
            DirectSample sample(evt);
            sample.wi = evt.toWorld(bsdfSample.wi);

            // Evaluate light pdf for BSDF sample
            // If BSDF is specular, it has a delta distribution, so avoid using MIS
            if (!hasType(bsdfSample.type, BSDFType::SPECULAR)) {
                pdfLight = light.pdfDirect(sample);
                if (pdfLight == 0)
                    return Ldir;

                misWeight = powerHeuristicBetaTwo(bsdfSample.pdf, pdfLight, 1, 1);
            }

            // Evaluate light emission
            Color Li = Color::BLACK;
            Ray   ray = evt.spawnRay(sample.wi);

            SurfaceEvent lightIt;
            if (_scene->intersectRay(ray, &lightIt)) {
                // If it is the same light, compute self emission
                if (lightIt.obj()->areaLight() == &light)
                    Li = lightIt.Le(-sample.wi);
            } else {
                // Evaluate infinite light (if that is the case)

                //Li = light.evalInfinite
            }

            if (!Li.isBlack())
                Ldir += bsdfF * Li * misWeight / bsdfSample.pdf;
        }
    }

    return Ldir;
}