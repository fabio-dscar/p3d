#pragma once

#include <Microfacet.h>
#include <BSDF.h>
#include <Records.h>
#include <Fresnel.h>

namespace Photon {

    class RoughLayered : public BSDF {
    public:
        RoughLayered(const BSDF& innerBsdf, Float intIor, Float extIor) 
            : BSDF(BSDFType(GLOSSY | REFLECTION | REFRACTION)),
            _inner(&innerBsdf), _eta(extIor / intIor) { }

        Float eta() const {
            return _eta;
        }

        Float evalPdf(const BSDFSample& sample) const {
            Vec3 wi = sample.wi;
            Vec3 wo = sample.wo;

            if (!Frame::sameSide(wo, wi))
                return 0;

            Vec3 wh = normalize(wo + wi) * Math::sign(Frame::cosTheta(wi));

            // Compute pdf of upper surface
            Float pdf = _dist.evalPdf(wo, wh) / (4.0 * dot(wo, wh));
        
            // Compute pdf of lower surface
            if (_inner) {
                Float eta = _eta;
                if (Frame::cosTheta(wi) < 0)
                    eta = 1.0 / eta;

                BSDFSample innerSample(sample);
                innerSample.wi = Frame::refract(wi, Normal(wh), eta);
                innerSample.wo = Frame::refract(wo, Normal(wh), eta);

                Float pdf2 = _inner->evalPdf(sample);
                
                Float F12 = fresnelDielectric(eta, Frame::cosTheta(wi));

                pdf *= F12;
                pdf += (1 - F12) * pdf2; // F12 * pdf1 + (1 - F12) * pdf2;
            }

            return pdf;
        }

        Color eval(const BSDFSample& sample) const {
            Color fr = Color::BLACK;

            Vec3 wi = sample.wi;
            Vec3 wo = sample.wo;

            Float eta = _eta;
            if (Frame::cosTheta(wi) < 0)
                eta = 1.0 / eta;

            if (hasType(sample.type, REFLECTION) && Frame::sameSide(wi, wo)) {
                Float cosThetaI = Frame::absCosTheta(wi);
                Float cosThetaO = Frame::absCosTheta(wo);

                if (cosThetaI == 0 || cosThetaO == 0)
                    return Color::BLACK;

                Vec3 wh = normalize(wi + wo) * Math::sign(Frame::cosTheta(wi));
                if (wh.isZero())
                    return Color::BLACK;

                Float F12 = fresnelDielectric(eta, Math::absDot(wi, wh));
                Float T12 = (1 - F12);

                Float D = _dist.evalD(wh);
                Float G = _dist.evalG(wi, wo, wh);

                Float fr1 = F12 * D * G / (4.0 * cosThetaI * cosThetaO);

                fr += fr1;
            }

            // If there is an inner material, add its contribution
            if (_inner) {
                Vec3 wh = normalize(wi + wo) * Math::sign(Frame::cosTheta(wi));
                if (wh.isZero())
                    return Color::BLACK;

                BSDFSample innerSample(sample);
                innerSample.wi = Frame::refract(wi, Normal(wh), eta);
                innerSample.wo = Frame::refract(wo, Normal(wh), eta);
                if (innerSample.wo.isZero() || innerSample.wi.isZero())
                    return fr; // TIR

                // Compute absorption
                Float cosI = Frame::cosTheta(innerSample.wi);
                Float cosO = Frame::cosTheta(innerSample.wo);
                Float l = _thickness * (1.0 / cosI + 1.0 / cosO);
                Color a = exp(-l * _absorption);

                Vec3 wh = normalize(innerSample.wi + innerSample.wo);
                wh *= Math::sign(Frame::cosTheta(wi));

                Float F21 = fresnelDielectric(1.0 / eta, Math::dot(wh, innerSample.wi));
                Float T21 = (1 - F21);

                Float G = _dist.evalG(wi, wo, wh);
                Float t = (1 - G) + T21 * G;

                Float F12 = fresnelDielectric(eta, Math::absDot(wi, wh));
                Float T12 = (1 - F12);

                fr += T12 * _inner->eval(sample) * a * t;
            }

            return fr;
        }
        
        Color sample(const Point2& rand, BSDFSample* sample) const {
            Point2 uRand = rand;

            Vec3 wo = sample->wo;

            Float F12 = fresnelDielectric(_eta, Frame::cosTheta(wo));

            Float refl = F12;
            bool sampleRefl = true;
            if (uRand.x < refl) {
                uRand.x = uRand.x / refl;
            } else {
                uRand.x = (uRand.x - refl) / (1 - refl);
                sampleRefl = false;
            }


        }

    private:
        MicrofacetDist _dist;
        Float _eta;
        Float _thickness;
        Color _absorption;

        const BSDF* _inner;
    };

}