#pragma once

#include <BSDF.h>

namespace Photon {

    class ThinSpecular : public BSDF {
    public:
        ThinSpecular();
        ThinSpecular(Float intIor, Float extIor);
        ThinSpecular(Float intIor, Float extIor, const Color& refl, const Color& refr);

        Float evalPdf(const BSDFSample& sample) const;
        Color eval   (const BSDFSample& sample) const;
        Color sample (const Point2& rand, BSDFSample* sample) const;

        Float eta() const;
    private:
        Color _refl;
        Color _refr;
        Float _intIor;
        Float _extIor;
        Float _eta;
    };

}