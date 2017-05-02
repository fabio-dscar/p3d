#pragma once

#include <Microfacet.h>
#include <BSDF.h>
#include <Records.h>
#include <Fresnel.h>

namespace Photon {

    class MicrofacetReflection : public BSDF {
    public:
        MicrofacetReflection()
            : BSDF(BSDFType(GLOSSY | REFLECTION)),
            _intIor(1.5), _extIor(1),
            _dist(GGX, Vec2(0.4, 0.4)), _refl(1),
            _eta(1.0 / 1.5) {}

        MicrofacetReflection(DistributionType type, Float alpha, Float intIor, Float extIor)
            : BSDF(BSDFType(GLOSSY | REFLECTION)),
            _intIor(intIor), _extIor(extIor),
            _dist(type, Vec2(alpha)), _refl(1),
            _eta(extIor / intIor) {}

        MicrofacetReflection(DistributionType type, const Vec2& alpha, Float intIor, Float extIor)
            : BSDF(BSDFType(GLOSSY | REFLECTION)),
            _intIor(intIor), _extIor(extIor),
            _dist(type, alpha), _refl(1),
            _eta(extIor / intIor) {}

        Float eta() const {
            return _extIor / _intIor;
        }

        Float evalPdf(const BSDFSample& sample) const {
            if (!hasType(sample.type, BSDFType(GLOSSY | REFLECTION)))
                return 0;

            Vec3 wo = sample.wo;
            Vec3 wi = sample.wi;

            // Return if wi and wo are not on the same hemisphere
            if (!Frame::sameSide(wo, wi)) 
                return 0;

            Vec3 wh = normalize(wo + wi);

            return _dist.evalPdf(wo, wh) / (4.0 * dot(wo, wh));
        }

        Color eval(const BSDFSample& sample) const {
            if (!hasType(sample.type, BSDFType(GLOSSY | REFLECTION)))
                return Color::BLACK;

            Vec3 wo = sample.wo;
            Vec3 wi = sample.wi;

            // Return if wi and wo are not on the same hemisphere
            if (!Frame::sameSide(wo, wi))
                return Color::BLACK;

            Float cosThetaO = Frame::absCosTheta(wo);
            Float cosThetaI = Frame::absCosTheta(wi);

            if (cosThetaI == 0 || cosThetaO == 0)
                return Color::BLACK;

            // Build the half-vector
            Vec3 wh = wi + wo;
            if (wh.isZero())
                return Color::BLACK;

            wh = normalize(wh);

            Float F = fresnelDielectric(_intIor, _extIor, dot(wi, wh));
            Float D = _dist.evalD(wh);
            Float G = _dist.evalG(wo, wi, wh);

            return _refl * D * G * F / (4.0 * cosThetaI * cosThetaO);
        }

        Color sample(const Point2& rand, BSDFSample* sample) const {
            if (!hasType(sample->type, BSDFType(GLOSSY | REFLECTION)))
                return Color::BLACK;

            Vec3 wo = sample->evt->wo;

            if (!Frame::onPositiveHemisphere(wo))
                return Color::BLACK;

            // Sample normal from microfacet distribution
            Vec3 wh = _dist.sample(wo, rand);

            sample->wi = Frame::reflect(wo, wh);

            if (Frame::cosTheta(sample->wi) == 0 ||
               !Frame::sameSide(sample->wi, wo))
                return Color::BLACK;

            sample->type = BSDFType(REFLECTION | GLOSSY);
            sample->pdf  = _dist.evalPdf(wo, wh) / (4.0 * dot(wo, wh));

            if (sample->pdf == 0)
                return Color::BLACK;

            return eval(*sample);
        }

    private:
        MicrofacetDist _dist;
        Color _refl;
        Float _intIor;
        Float _extIor;
        Float _eta;
    };

}
