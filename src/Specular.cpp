#include <Specular.h>

#include <Frame.h>
#include <Fresnel.h>
#include <Records.h>

using namespace Photon;

Specular::Specular()
    : BSDF(BSDFType(REFLECTION |REFRACTION | SPECULAR)),
    _intIor(1.5), _extIor(1),
    _eta(1.0 / 1.5),
    _refl(1), _refr(1) {}

Specular::Specular(Float intIor, Float extIor)
    : BSDF(BSDFType(REFLECTION | REFRACTION | SPECULAR)),
    _intIor(intIor), _extIor(extIor),
    _eta(extIor / intIor),
    _refl(1), _refr(1) {}

Specular::Specular(Float intIor, Float extIor, const Color& refl, const Color& refr)
    : BSDF(BSDFType(REFLECTION | REFRACTION | SPECULAR)),
    _intIor(intIor), _extIor(extIor),
    _eta(extIor / intIor),
    _refl(refl), _refr(refr) {}

Float Specular::eta() const {
    return _extIor / _intIor;
}

Float Specular::evalPdf(const BSDFSample& sample) const {
    return 0;
}

Color Specular::eval(const BSDFSample& sample) const {
    return Color::BLACK;
}

Color Specular::sample(const Point2& rand, BSDFSample* sample) const {
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

        return _refl * fresn / Frame::absCosTheta(sample->wi);
    } else {
        if (fresn == 1) {
            sample->pdf = 0;
            return Color::BLACK;
        }

        Float eta = _extIor / _intIor;
        if (cosI < 0) // If we are leaving the surface
            eta = 1.0 / eta;  // Swap Iors

        // Refraction
        sample->type = BSDFType(REFRACTION | SPECULAR);
        sample->wi   = Frame::refract(wo, eta, cosT);
        sample->pdf  = both ? (1 - fresn) : 1.0;
        sample->eta  = eta;

        // Scale for non-symmetry of transport
        Float scale = (1.0 - fresn);
        if (sample->transp == Transport::RADIANCE)
            scale *= (1.0 / eta) * (1.0 / eta);

        return _refr * scale / Frame::absCosTheta(sample->wi);
    }
}