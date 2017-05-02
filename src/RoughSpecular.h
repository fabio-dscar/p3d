#pragma once

#include <Microfacet.h>
#include <BSDF.h>
#include <Records.h>
#include <Fresnel.h>

namespace Photon {

    class RoughSpecular : public BSDF {
    public:
        RoughSpecular() 
            : BSDF(BSDFType(GLOSSY | 
                            REFLECTION | 
                            REFRACTION)),
              _dist(GGX, Vec2(0.4, 0.4)),
              _refl(1), _refr(1),
              _eta(1.5 / 1.0) { }

        RoughSpecular(DistributionType type, Float alpha, Float intIor, Float extIor)
            : BSDF(BSDFType(GLOSSY |
                            REFLECTION |
                            REFRACTION)),
            _dist(type, Vec2(alpha)),
            _refl(1), _refr(1),
            _eta(intIor / extIor) { }

        Float evalPdf(const BSDFSample& sample) const {
            bool refl = hasType(sample.type, BSDFType::REFLECTION);
            bool refr = hasType(sample.type, BSDFType::REFRACTION);

            if (!hasType(sample.type, BSDFType::GLOSSY) ||
               (!refl && !refr)) {
                return 0;
            }

            Vec3 wi = sample.wi;
            Vec3 wo = sample.wo;
           
            Vec3 h;

            bool reflect = Frame::cosTheta(wi) * Frame::cosTheta(wo) > 0;
            if (reflect) {
                h = normalize(wo + wi);
            } else {
                Float eta = Frame::cosTheta(wi) < 0 ? _eta : (1.0 / _eta);
                h = normalize(wo + eta * wi);
            }

            h *= Math::sign(Frame::cosTheta(h));

            Float jacobian;
            if (reflect) {
                jacobian = 1.0f / (4.0f * dot(wo, h));
            } else {
                /* Calculate the transmission half-vector */
                Float eta = Frame::cosTheta(wi) > 0 ? _eta : 1.0 / _eta;

                /* Jacobian of the half-direction mapping */
                Float sqrtDenom = dot(wo, h) + eta * dot(wi, h);
                jacobian = (eta * eta * dot(wi, h)) / (sqrtDenom * sqrtDenom);
            }
            
            h *= Math::sign(Frame::cosTheta(h));

            /* Evaluate the microfacet model sampling density function */
            Float distPdf = _dist.evalPdf(wi * Math::sign(Frame::cosTheta(wi)), h);

            if (refr && refl) {
                Float cosT;
                Float F = fresnelDielectric(dot(wi, h), _eta, cosT);
                distPdf *= reflect ? F : (1 - F);
            }

            return distPdf * std::abs(jacobian);
        }

        Color eval(const BSDFSample& sample) const {           
            if (!hasType(sample.type, BSDFType::GLOSSY) ||
               (!hasType(sample.type, BSDFType::REFLECTION) &&
                !hasType(sample.type, BSDFType::REFRACTION))) {
                return Color::BLACK;
            }

            Vec3 wi = sample.wi;
            Vec3 wo = sample.wo;

            if (Frame::cosTheta(wi) == 0 || Frame::cosTheta(wo) == 0)
                return Color::BLACK;

            Vec3 h;

            bool refl = Frame::cosTheta(wi) * Frame::cosTheta(wo) > 0;
            if (refl) {
                h = normalize(wo + wi);
            } else {
                Float eta = Frame::cosTheta(wi) < 0 ? _eta : (1.0 / _eta);
                h = normalize(wo + eta * wi);
            }

            h *= Math::sign(Frame::cosTheta(h));

            Float D = _dist.evalD(h);
            if (D == 0)
                return Color::BLACK;

            Float cosT;
            Float F = fresnelDielectric(_eta, dot(wo, h), cosT);
            Float G = _dist.evalG(wo, wi, h);

            if (refl) {
                // Reflection
                return _refl * F * D * G / (4.0 * Frame::absCosTheta(wi) * Frame::absCosTheta(wo));
            } else {
                // Refraction
                Float eta = Frame::cosTheta(wo) > 0.0f ? _eta : (1.0 / _eta);

                /* Calculate the total amount of transmission */
                Float sqrtDenom = dot(wo, h) + eta * dot(wi, h);
                Float value = ((1 - F) * D * G * eta * eta * absDot(wi, h) * absDot(wo, h)) /
                               (Frame::cosTheta(wi) * Frame::cosTheta(wo) * sqrtDenom * sqrtDenom);

                Float factor = 1;
                if (sample.transp == Transport::RADIANCE)
                    factor *= (1.0 / eta) * (1.0 / eta);

                return _refr * std::abs(value * factor * factor);
            }
        }

        Color sample(const Point2& rand, BSDFSample* sample) const {
            Color weight = Color(1);

            bool refl = hasType(sample->type, BSDFType::REFLECTION);
            bool refr = hasType(sample->type, BSDFType::REFRACTION);

            if (!hasType(sample->type, BSDFType::GLOSSY) ||
               (!refl && !refr)) {
                sample->pdf = 0;
                return Color::BLACK;
            }

            Vec3 wo  = sample->evt->wo;
            Point2 u = rand;
            Normal n = _dist.sample(wo * (Float)Math::sign(Frame::cosTheta(wo)), u);
            Float distPdf = _dist.evalPdf(wo, n);
            if (distPdf == 0)
                return Color::BLACK;

            sample->pdf = distPdf;

            Float cosT;
            Float fresn = fresnelDielectric(_eta, dot(wo, n), cosT);

            //refr = false; // Remove
            if (refl && refr) {
                if (u.y > fresn) {
                    refl = false;
                    sample->pdf *= (1.0 - fresn);
                } else
                    sample->pdf *= fresn;
            } else {
                weight *= (refl ? fresn : (1.0 - fresn));
            }

            Float jacobScale = 0;
            if (refl) {
                sample->type = BSDFType(REFLECTION | GLOSSY);
                sample->wi   = Frame::reflect(wo, n);
                sample->eta  = 1.0;

                if (Frame::cosTheta(wo) * Frame::cosTheta(sample->wi) <= 0) {
                    sample->pdf = 0;
                    return Color::BLACK;
                }
                
                weight *= _refl;
                jacobScale = 1.0 / (4.0 * dot(wo, n));
            } else {
                sample->type = BSDFType(REFRACTION | GLOSSY);
                sample->wi   = Frame::refract(wo, n, _eta, cosT);   

                if (Frame::cosTheta(wo) * Frame::cosTheta(sample->wi) >= 0) {
                    sample->pdf = 0;
                    return Color::BLACK;
                }

                Float eta = _eta;
                if (cosT > 0)
                    eta = 1.0 / eta;

                sample->eta = eta;

                if (sample->transp == Transport::RADIANCE)
                    weight *= _refr * (1.0 / eta) * (1.0 / eta);

                Float sqrtDenom = dot(wo, n) + eta * dot(sample->wi, n);
                jacobScale = (dot(sample->wi, n) * eta * eta) /
                             (sqrtDenom * sqrtDenom);
            }

            sample->pdf *= std::abs(jacobScale);

            // Phong does not sample visible normals
            // -> Heitz, d'Eon, 2014
            if (_dist.type() != PHONG)
                weight *= _dist.evalG1(sample->wi, n);
            else
                weight *= std::abs(_dist.evalD(n) * _dist.evalG(wo, sample->wi, n)
                                   * dot(wo, n) / (distPdf * Frame::cosTheta(wo)));

            return weight;
        }

    private:
        MicrofacetDist _dist;
        Color _refl;
        Color _refr;
        Float _intIor;
        Float _extIor;
        Float _eta;
    };

}