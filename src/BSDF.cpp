#include <BSDF.h>
#include <Records.h>
#include <Sampling.h>

using namespace Photon;

Float BSDF::eta() const {
    return 1.0;
}

BSDFType BSDF::type() const {
    return _type;
}

bool BSDF::isType(BSDFType type) const {
    return (_type & type) == type;
}

Float BSDF::evalPdf(const BSDFSample& sample) const {
    if (Frame::sameSide(sample.wi, sample.wo))
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
    sample->wi.z *= Math::sign(sample->evt->wo.z);

    sample->pdf  = evalPdf(*sample);
    sample->type = _type;

    return eval(*sample);
}

Float NullBSDF::evalPdf(const BSDFSample& sample) const {
    return 0;
}

Color NullBSDF::eval(const BSDFSample& sample) const {
    return Color::BLACK;
}

Color NullBSDF::sample(const Point2& rand, BSDFSample* sample) const {
    sample->pdf = 0;
    return Color::BLACK;
}
