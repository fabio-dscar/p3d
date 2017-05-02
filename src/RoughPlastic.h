#pragma once

#include <PhotonMath.h>
#include <Microfacet.h>
#include <BSDF.h>
#include <Records.h>
#include <Sampling.h>

namespace Photon {

    class RoughPlastic : public BSDF {
    public:
        RoughPlastic(Float alpha) 
            : BSDF(BSDFType(BSDFType::GLOSSY | 
                            BSDFType::DIFFUSE | 
                            BSDFType::REFLECTION)),
             _dist(GGX, Vec2(alpha, alpha)) { }

        Float evalPdf(const BSDFSample& sample) const {

        }

        Color eval(const BSDFSample& sample) const {

        }

        Color sample(const Point2& rand, BSDFSample* sample) const {

            Vec3 wo = sample->evt->wo;

            bool glossy;
            if (glossy) {
                Normal n = _dist.sample(wo, rand);

                sample->wi = Frame::reflect(wo, n);
                sample->type = BSDFType(GLOSSY | REFLECTION);
            } else {
                sample->wi = sampleCosHemisphere(rand);
                sample->type = BSDFType(DIFFUSE | REFLECTION);
            }

            sample->pdf = evalPdf(*sample);

            return eval(*sample);
        }

    private:
        MicrofacetDist _dist;
    };
}