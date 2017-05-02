#include <AshikhminShirley.h>

#include <Fresnel.h>
#include <Sampling.h>
#include <Records.h>
#include <Frame.h>

using namespace Photon;

AshikhminShirley::AshikhminShirley()
    : BSDF(BSDFType(DIFFUSE | GLOSSY | REFLECTION)),
    _diff(1), _spec(1), _dist(GGX, Vec2(0.02, 0.02)) {}

AshikhminShirley::AshikhminShirley(DistributionType distType, const Vec2& alpha, const Color& diff, const Color& spec)
    : BSDF(BSDFType(DIFFUSE | GLOSSY | REFLECTION)),
    _diff(diff), _spec(spec), _dist(distType, alpha) {}

Float AshikhminShirley::evalPdf(const BSDFSample& sample) const {
    Vec3 wi = sample.wi;
    Vec3 wo = sample.wo;

    if (!Frame::sameSide(wo, wi))
        return 0;

    Vec3 wh = normalize(wo + wi);

    Float glossyPdf = _dist.evalPdf(wo, wh) / (4.0 * dot(wo, wh));
    Float diffPdf   = Frame::absCosTheta(wi) * INVPI;

    return (diffPdf + glossyPdf) / 2;
}

Color AshikhminShirley::eval(const BSDFSample& sample) const {
    Vec3 wi = sample.wi;
    Vec3 wo = sample.wo;

    if (!Frame::sameSide(wi, wo))
        return Color::BLACK;

    // Compute diffuse contribution
    Float pWi = (1.0 - 0.5 * Frame::absCosTheta(wi));
    pWi *= (pWi * pWi) * (pWi * pWi);

    Float pWo = (1.0 - 0.5 * Frame::absCosTheta(wo));
    pWo *= (pWo * pWo) * (pWo * pWo);

    Color diff = ((28.0 / (23.0 * PI)) * _diff *
        (Color(1) - _spec) * (1 - pWi) * (1 - pWo));

    // Compute specular contribution
    Vec3 wh = wi + wo;
    if (wh.isZero())
        return Color::BLACK;

    wh.normalize();

    Float max = std::max(Frame::absCosTheta(wi), Frame::absCosTheta(wo));
    Color Fr  = fresnelSchlick(_spec, dot(wi, wh));
    Float D   = _dist.evalD(wh);

    Color spec = Fr * D / (4.0 * absDot(wi, wh) * max);

    return diff + spec;
}

Color AshikhminShirley::sample(const Point2& rand, BSDFSample* sample) const {
    Point2 uRand = rand;
    if (uRand.x < 0.5) {
        uRand.x = 2.0f * uRand.x; // Reuse sample

        sample->type = BSDFType(DIFFUSE | REFLECTION);
        sample->wi   = sampleCosHemisphere(uRand);

        sample->wi.z *= Math::sign(Frame::cosTheta(sample->wi));

    } else {
        uRand.x = 2.0f * (uRand.x - 0.5f); // Reuse sample

        Vec3 wh = _dist.sample(sample->wo, uRand);

        sample->type = BSDFType(GLOSSY | REFLECTION);
        sample->wi   = Frame::reflect(sample->wo, wh);

        if (!Frame::sameSide(sample->wo, sample->wi)) {
            sample->pdf = 0;
            return Color::BLACK;
        }
    }

    sample->pdf = evalPdf(*sample);

    return eval(*sample);
}