#pragma once

#include <BSDF.h>
#include <Records.h>
#include <Fresnel.h>
#include <Microfacet.h>

namespace Photon {

    class RoughLayered : public BSDF {
    public:
        RoughLayered(const BSDF& innerBsdf) 
            : BSDF(BSDFType(GLOSSY | REFLECTION | REFRACTION)),
            _inner(&innerBsdf) { }

        Float evalPdf(const BSDFSample& sample) const {

        }

        Color eval(const BSDFSample& sample) const {
            Color res(0);

            Vec3 wi = sample.wi;
            Vec3 wo = sample.wo;

            Float fresn = fresnelDielectric(_eta, Frame::cosTheta(wi));


            if (_inner) {
                BSDFSample innerSample(sample);
                innerSample.wi = Frame::refract(wi, Normal(0, 0, 1), _eta);
                innerSample.wo = Frame::refract(wo, Normal(0, 0, 1), _eta);

                Color nested = _inner->eval(sample);

                // Compute absorption
                Float cosI = Frame::cosTheta(innerSample.wi);
                Float cosO = Frame::cosTheta(innerSample.wo);
                Float l = _thickness * (1.0 / cosI + 1.0 / cosO);
                Color intensity = exp(-l * _absorption);

                res += intensity * nested;
            }

            return res;
        }
        
        Color sample(const Point2& rand, BSDFSample* sample) const {

        }

    private:
        MicrofacetDist _dist;
        Float _eta;
        Float _thickness;
        Color _absorption;
        const BSDF* _inner;
    };

}