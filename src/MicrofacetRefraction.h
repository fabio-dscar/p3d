#pragma once

#include <Microfacet.h>
#include <BSDF.h>
#include <Records.h>
#include <Fresnel.h>

namespace Photon {

    class MicrofacetRefraction : public BSDF {
    public:
        MicrofacetRefraction()
            : BSDF(BSDFType(GLOSSY | REFRACTION)),
            _dist(GGX, Vec2(0.4, 0.4)), _refr(1),
            _eta(1.0 / 1.5) {}

        MicrofacetRefraction(DistributionType type, Float alpha, Float intIor, Float extIor)
            : BSDF(BSDFType(GLOSSY | REFRACTION)),
            _dist(type, Vec2(alpha)), _refr(1),
            _eta(extIor / intIor) {}

        MicrofacetRefraction(DistributionType type, const Vec2& alpha, Float intIor, Float extIor)
            : BSDF(BSDFType(GLOSSY | REFRACTION)),
            _dist(type, alpha), _refr(1),
            _eta(extIor / intIor) {}

        Float eta() const {
            return _extIor / _intIor;
        }

        Float evalPdf(const BSDFSample& sample) const {
            if (!hasType(sample.type, BSDFType(GLOSSY | REFRACTION)))
                return 0;

            Vec3 wo = sample.wo;
            Vec3 wi = sample.wi;

            // wi and wo must be on different hemispheres
            if (Frame::sameSide(wi, wo))
                return 0;

            // Note that here we are evaluating the pdf for sampled wi,
            // so the eta will be chosen contrary to how we do it in sample()
            Float eta = Frame::cosTheta(wo) > 0 ? (1.0 / _eta) : _eta;

            // Compute half-vector
            Vec3 wh = normalize(wo + wi * eta);
            if (wh.z < 0)
                wh = -wh;

            // Compute Jacobian factor
            Float denom = dot(wo, wh) + eta * dot(wi, wh);
            Float jacobian = eta * eta * absDot(wi, wh) / (denom * denom);

            return _dist.evalPdf(wo, wh) * jacobian;
        }

        Color eval(const BSDFSample& sample) const {
            if (!hasType(sample.type, BSDFType(GLOSSY | REFRACTION)))
                return Color::BLACK;

            Vec3 wo = sample.wo;
            Vec3 wi = sample.wi;

            Float cosO = Frame::cosTheta(wo);
            Float cosI = Frame::cosTheta(wi);

            if (Frame::sameSide(wi, wo) || cosO * cosI == 0)
                return Color::BLACK;

            // Note that here we are evaluating the sampled wi,
            // so the eta will be chosen contrary to how we do it in sample()
            Float eta = Frame::cosTheta(wo) > 0 ? (1.0 / _eta) : _eta;

            // Compute the half-vector
            Vec3 wh = normalize(wo + eta * wi);
            if (wh.z < 0)
                wh = -wh;

            // Compute fresnel component
            Float fresn = fresnelDielectric(eta, dot(wi, wh));
            
            Float scale = 1;
            if (sample.transp == Transport::RADIANCE)
                scale = (1.0 / eta);

            // Compute distribution and shadowing
            Float T = (1.0 - fresn);
            Float D = _dist.evalD(wh);
            Float G = _dist.evalG(wo, wi, wh);         
            Float denom = dot(wo, wh) + eta * dot(wi, wh);
            Float jacobian = eta * eta * absDot(wi, wh) / (denom * denom);

            return _refr * T * D * G * jacobian * absDot(wo, wh) * 
                         scale * scale / (abs(cosI) * abs(cosO));
        }

        Color sample(const Point2& rand, BSDFSample* sample) const {
            if (!hasType(sample->type, BSDFType(GLOSSY | REFRACTION)))
                return Color::BLACK;

            Vec3 wo = sample->evt->wo;

            Float cos = Frame::cosTheta(wo);
            if (cos == 0)
                return Color::BLACK;

            // Sample a normal, with 'wo' on the positive hemisphere
            Normal wh = _dist.sample(wo * sign(cos), rand);

            // Set the normal on the correct hemisphere
            wh *= sign(cos);  

            // Compute transmission vector
            Float eta = cos > 0 ? _eta : (1.0 / _eta);
            Vec3 wt = Frame::refract(wo, wh, eta);
            if (wt.isZero()) // Total internal reflection check
                return Color::BLACK;

            sample->type = BSDFType(GLOSSY | REFRACTION);
            sample->wi   = wt;
            sample->eta  = eta;
            sample->pdf  = evalPdf(*sample);

            return eval(*sample);
        }

    private:
        MicrofacetDist _dist;
        Color _refr;
        Float _intIor;
        Float _extIor;
        Float _eta;
    };

}