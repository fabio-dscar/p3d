#include <ThinSpecular.h>

#include <Frame.h>
#include <Fresnel.h>
#include <Records.h>

using namespace Photon;

ThinSpecular::ThinSpecular()
    : BSDF(BSDFType(REFLECTION | REFRACTION | SPECULAR)),
    _intIor(1.5), _extIor(1),
    _eta(1.0 / 1.5),
    _refl(1), _refr(1) {}

ThinSpecular::ThinSpecular(Float intIor, Float extIor)
    : BSDF(BSDFType(REFLECTION | REFRACTION | SPECULAR)),
    _intIor(intIor), _extIor(extIor),
    _eta(intIor / extIor),
    _refl(1), _refr(1) {}

ThinSpecular::ThinSpecular(Float intIor, Float extIor, const Color& refl, const Color& refr)
    : BSDF(BSDFType(REFLECTION | REFRACTION | SPECULAR)),
    _intIor(intIor), _extIor(extIor),
    _eta(intIor / extIor),
    _refl(refl), _refr(refr) {}

Float ThinSpecular::eta() const {
    return _extIor / _intIor;
}

Float ThinSpecular::evalPdf(const BSDFSample& sample) const {
    return 0;
}

Color ThinSpecular::eval(const BSDFSample& sample) const {
    return Color::BLACK;
}

Color ThinSpecular::sample(const Point2& rand, BSDFSample* sample) const {
    bool refl = hasType(sample->type, REFLECTION) && !_refl.isBlack();
    bool refr = hasType(sample->type, REFRACTION) && !_refr.isBlack();

    // Return if none of the lobes is requested or available
    if (!hasType(sample->type, BSDFType::SPECULAR) ||
        (!refl && !refr)) {
        sample->pdf = 0;
        return Color::BLACK;
    }

    Float cosT;
    Vec3  wo    = sample->evt->wo;
    Float cosI  = Frame::cosTheta(wo);
    Float fresn = fresnelDielectric(_intIor, _extIor, cosI, cosT);

    // Account for interreflections on the thin sheet
    // This is a sum of the geometric series of interreflections
    if (fresn < 1)
        fresn += (1.0 - fresn) * (1.0 - fresn) * fresn / (1.0 - fresn * fresn);

    // Choose BSDF lobe
    // - Choose randomly if we want to sample both
    // - Choose the specific if only one is requested
    Float u = rand.x;
    bool both = refr && refl;
    if (!both)
        u = refl ? 0 : 1;

    if (u < fresn) {
        // Reflection
        sample->type = BSDFType(REFLECTION | SPECULAR);
        sample->wi   = Frame::reflect(wo);
        sample->pdf  = both ? fresn : 1.0;

        Float scale = both ? 1.0 : fresn;

        return _refl * scale / Frame::absCosTheta(sample->wi);
    } else {
        if (fresn == 1) {
            sample->pdf = 0;
            return Color::BLACK;
        }

        // Refraction
        sample->type = BSDFType(REFRACTION | SPECULAR);
        sample->wi   = -wo;  // Refraction from very thin sheet
        sample->pdf  = both ? (1.0 - fresn) : 1.0;

        Float scale = both ? 1.0 : (1.0 - fresn);

        return _refr * scale / Frame::absCosTheta(sample->wi);
    }
}