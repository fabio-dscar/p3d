#include <Mirror.h>

#include <Records.h>
#include <Frame.h>

using namespace Photon;

Mirror::Mirror()
    : BSDF(BSDFType(BSDFType::REFLECTION |
                    BSDFType::SPECULAR)),
    _refl(1.0) {}

Mirror::Mirror(const Color& refl)
    : BSDF(BSDFType(BSDFType::REFLECTION |
                    BSDFType::SPECULAR)),
    _refl(refl) {}

Float Mirror::evalPdf(const BSDFSample& sample) const {
    return 0;
}

Color Mirror::eval(const BSDFSample& sample) const {
    return Color::BLACK;
}

Color Mirror::sample(const Point2& rand, BSDFSample* sample) const {
    if (!hasType(sample->type, _type)) {
        sample->pdf = 0;
        return Color::BLACK;
    }

    sample->type = BSDFType(BSDFType::REFLECTION | BSDFType::SPECULAR);
    sample->wi = Frame::reflect(sample->evt->wo);
    sample->pdf = 1.0;

    return _refl / Frame::absCosTheta(sample->wi);
}