#include <BSDF.h>
#include <Records.h>
#include <Sampling.h>

using namespace Photon;

Float BSDF::evalPdf(const BSDFSample& sample) const {
    if ((sample.wi.z * sample.evt->wo().z) > 0)
        return pdfCosHemisphere(Frame::absCosTheta(sample.wi));

    return 0;
}

Color BSDF::sample(const Point2& rand, BSDFSample* sample) const {
    if (!hasType(sample->type, _type)) {
        sample->pdf = 0;
        return Color::BLACK;
    }

    // By default, BSDFs sample a cosine distribution
    sample->wi = sampleCosHemisphere(rand);

    // Make hemispheres agree
    sample->wi.z *= Math::sign(sample->evt->wo().z);

    sample->pdf  = evalPdf(*sample);
    sample->type = _type;

    return eval(*sample);
}