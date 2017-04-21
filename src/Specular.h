#pragma once

#include <BSDF.h>
#include <Frame.h>
#include <Fresnel.h>
#include <Records.h>

namespace Photon {

    class Specular : public BSDF {
    public:
        Specular() 
            : BSDF(BSDFType(BSDFType::REFLECTION | 
                            BSDFType::REFRACTION | 
                            BSDFType::SPECULAR)), 
            _intEta(1.5), _extEta(1.0),
            _refl(1), _refr(1) { }

        Specular(Float intEta, Float extEta)
            : BSDF(BSDFType(BSDFType::REFLECTION |
                            BSDFType::REFRACTION |
                            BSDFType::SPECULAR)),
            _intEta(intEta), _extEta(extEta),
            _refl(1), _refr(1) {}

        Float evalPdf(const BSDFSample& sample) const {
            return 0;
        }

        Color eval(const BSDFSample& sample) const {
            // Cannot eval deltas
            return Color::BLACK;
        }

        Color sample(const Point2& rand, BSDFSample* sample) const {
            /*if (!hasType(sample->type, _type)) {
                sample->pdf = 0;
                return Color::BLACK;
            }*/

            Vec3  wo       = sample->evt->wo();
            Float cosTheta = Frame::cosTheta(wo);
            Float fresn    = fresnelDielectric(_intEta, _extEta, cosTheta);
            
            // Choose BSDF lobe
            // - Choose randomly if we want to sample both
            // - Choose the specific if only one is requested
            Float sampleRefl = rand.x;

            if (!hasType(sample->type, BSDFType::REFLECTION))
                sampleRefl = 1;
            else if (!hasType(sample->type, BSDFType::REFRACTION))
                sampleRefl = 0;
            
            if (sampleRefl <= fresn) {
                // Reflection
                sample->type = BSDFType(BSDFType::REFLECTION | BSDFType::SPECULAR);
                sample->wi   = Frame::reflect(wo);
                sample->pdf  = fresn;

                Color scaledRefl = fresn * _refl;
                return scaledRefl / Frame::absCosTheta(sample->wi);
            } else {
                // Refraction
                sample->type = BSDFType(BSDFType::REFRACTION | BSDFType::SPECULAR);

                // Are we entering or leaving?
                Float eta1 = _intEta;
                Float eta2 = _extEta;
                if (cosTheta > 0) 
                    std::swap(eta1, eta2);

                // Make the normal face in the direction of wo
                Normal n = Normal(0, 0, 1);
                if (dot(wo, n) < 0)
                    n = -n;

                // Compute ray direction for specular transmission
                if (!Frame::refract(wo, &sample->wi, n, eta1 / eta2))
                    return Color::BLACK;

                Color scaledTrans = (1 - fresn) * _refr;
                sample->pdf = (1 - fresn);

                // Scale for non-symmetry of transport
                if (sample->transp == Transport::RADIANCE)
                    scaledTrans *= (eta1 * eta1) / (eta2 * eta2);

                return scaledTrans / Frame::absCosTheta(sample->wi);
            }
        }

    private:
        Color _refl;
        Color _refr;
        Float _intEta;
        Float _extEta;
    };

}