#pragma once

#include <BSDF.h>
#include <Records.h>
#include <Frame.h>
#include <Fresnel.h>

namespace Photon {
    
    class Mirror : public BSDF {
    public:
        Mirror() : BSDF(BSDFType(BSDFType::REFLECTION |
                                 BSDFType::SPECULAR)),
                   _refl(1.0) { }

        Float evalPdf(const BSDFSample& sample) const {
            return 0;
        }

        Color eval(const BSDFSample& sample) const {
            return Color::BLACK;
        }

        Color sample(const Point2& rand, BSDFSample* sample) const {
            sample->wi   = Frame::reflect(sample->evt->wo());
            sample->pdf  = 1.0;
            sample->type = BSDFType(BSDFType::REFLECTION | BSDFType::SPECULAR);

            return _refl / Frame::absCosTheta(sample->wi);
        }

    private:
        Color _refl;
    };

}