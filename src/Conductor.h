#pragma once

#include <BSDF.h>
#include <Records.h>
#include <Fresnel.h>

namespace Photon {

    class Conductor : public BSDF {
    public:
        Conductor(const Color& Ks, const Color& eta, const Color& k) 
            : BSDF(BSDFType(SPECULAR | REFLECTION)), _Ks(Ks), _eta(eta), _k(k) { }

        Float evalPdf(const BSDFSample& sample) const {
            return 0; // Delta distr
        }

        Color eval(const BSDFSample& sample) const {
            return Color::BLACK; // Delta distr
        }

        Color sample(const Point2& rand, BSDFSample* sample) const {
            if (!hasType(sample->type, _type) ||
                !Frame::onPositiveHemisphere(sample->evt->wo)) {
                sample->pdf = 0;
                return Color::BLACK;
            }

            Vec3 wo = sample->evt->wo;

            sample->type = BSDFType(SPECULAR | REFLECTION);
            sample->wi   = Frame::reflect(wo);
            sample->pdf  = 1;

            Color F = fresnelConductor(_eta, _k, Frame::cosTheta(wo));

            return _Ks * F;
        }

    private:
        Color _Ks;
        Color _eta;
        Color _k;
    };

}