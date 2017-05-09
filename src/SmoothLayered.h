#pragma once

#include <Microfacet.h>
#include <BSDF.h>
#include <Records.h>
#include <Fresnel.h>

namespace Photon {

    class SmoothLayered : public BSDF {
    public:
        SmoothLayered(const BSDF& innerBsdf, Color refl, Float intIor, Float extIor)
            : BSDF(BSDFType(GLOSSY | REFLECTION | REFRACTION)),
            _inner(&innerBsdf), _eta(extIor / intIor),
            _intIor(intIor), _extIor(extIor),
            _refl(refl), _thickness(0.05), _absorption(0.3, 0.3, 0.3) {}

        Float eta() const {
            return _eta;
        }

        Float evalPdf(const BSDFSample& sample) const {
            bool refl = hasType(sample.type, REFLECTION);

            Vec3 wo = sample.wo;
            Vec3 wi = sample.wi;

            Float cosT12;
            Float F12 = fresnelDielectric(_intIor, _extIor, Frame::cosTheta(wo), cosT12);

            if (refl) {
                return F12;
            } else {
                // Refract wi into the inner layer
                Vec3 wot = Frame::refract(wo, _eta, cosT12);
                if (wot.isZero())
                    return 0;  // TIR

                // Refract wo into the inner layer
                Vec3 wit = Frame::refract(wo, Normal(0, 0, 1), _eta);
                if (wit.isZero())
                    return 0;  // TIR

                // Sample the BSDF on the inner layer
                BSDFSample innerSample(sample);
                innerSample.type = ALL;
                innerSample.wi = -wit;
                innerSample.wo = -wot;

                return (1 - F12) * _inner->evalPdf(innerSample);
            }

        }

        Color eval(const BSDFSample& sample) const {
            Color fr = Color::BLACK;

            Vec3 wo = sample.wo;
            Vec3 wi = sample.wi;

            Float cosT12;
            Float F12 = fresnelDielectric(_intIor, _extIor, Frame::cosTheta(wi), cosT12);

            //Float cosT21;
            //Float F21 = fresnelDielectric(_intIor, _extIor, Frame::cosTheta(wo), cosT21);

            if (cosT12 == 0)
                return Color::BLACK;

            fr += _refl * F12 / Frame::absCosTheta(wi);

            if (hasType(sample.type, REFLECTION))
                return fr;

            // Refract wi into the inner layer
            Vec3 wit = Frame::refract(wi, _eta, cosT12);
            if (wit.isZero())
                return Color::BLACK;  // TIR

            // Refract wo into the inner layer
            Vec3 wot = Frame::refract(wo, Normal(0, 0, 1), _eta);
            if (wot.isZero())
                return Color::BLACK;  // TIR

            BSDFSample innerSample(sample);
            innerSample.type = ALL;
            innerSample.wi = -wit;
            innerSample.wo = -wot;

            Color inner = _inner->eval(innerSample);

            Float cosT21;
            Float F21 = fresnelDielectric(_intIor, _extIor, Frame::cosTheta(wot), cosT21);
            if (cosT21 == 0)
                return Color::BLACK;

            // Compute absorption
            Float cosI = Frame::cosTheta(innerSample.wi);
            Float cosO = Frame::cosTheta(innerSample.wo);
            Float l = _thickness * (1.0 / cosI + 1.0 / cosO);
            Color a = exp(-l * _absorption);

            fr += (1 - F21) * inner * a / Frame::absCosTheta(-innerSample.wo);

            return fr;
        }

        Color sample(const Point2& rand, BSDFSample* sample) const {
            Color fr = Color::BLACK;
            Point2 uRand = rand;

            Vec3 wo = sample->wo;

            Float cosT;
            Float F12 = fresnelDielectric(_intIor, _extIor, Frame::cosTheta(wo), cosT);

            Float refl = F12;
            bool sampleRefl = true;
            if (uRand.x < refl) {
                uRand.x = uRand.x / refl;  // Reuse sample
            } else {
                uRand.x = (uRand.x - refl) / (1 - refl);  // Reuse sample
                sampleRefl = false;
            }

            if (sampleRefl) {
                sample->type = BSDFType(REFLECTION | GLOSSY);
                sample->wi   = Frame::reflect(wo);
                sample->pdf  = F12;

                fr = _refl / Frame::absCosTheta(sample->wi);
            } else {
                // Refract into the inner layer
                Vec3 wt12 = Frame::refract(wo, _eta, cosT);
                if (wt12.isZero()) {
                    sample->pdf = 0;
                    return Color::BLACK;  // TIR
                }

                // Sample the BSDF on the inner layer
                BSDFSample innerSample(*sample);
                innerSample.wo = -wt12;

                Color inner = _inner->sample(rand, &innerSample);
                if (inner.isBlack()) {
                    sample->pdf = 0;
                    return Color::BLACK;
                }

                Float cosT2;
                Float F21 = fresnelDielectric(_intIor, _extIor, Frame::cosTheta(-innerSample.wi), cosT2);
                if (cosT2 == 0) {
                    sample->pdf = 0;
                    return Color::BLACK;  // TIR
                }

                // Refract the sampled direction out of the upper layer
                Vec3 wt21 = Frame::refract(-innerSample.wi, 1.0 / _eta, cosT2);

                // Compute absorption
                Float cosI = Frame::cosTheta(innerSample.wi);
                Float cosO = Frame::cosTheta(innerSample.wo);
                Float l = _thickness * (1.0 / cosI + 1.0 / cosO);
                Color a = exp(-l * _absorption);

                sample->type = BSDFType(REFRACTION | GLOSSY);
                sample->eta  = 1.0;
                sample->pdf  = (1 - F12) * innerSample.pdf;
                sample->wi   = wt21;
                
                fr = inner * a / Frame::absCosTheta(sample->wi);
            }

            return eval(*sample);
        }

    private:
        Color _refl;
        Float _intIor;
        Float _extIor;
        Float _eta;
        Float _thickness;
        Color _absorption;

        const BSDF* _inner;
    };

}