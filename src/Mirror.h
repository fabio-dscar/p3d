#pragma once

#include <BSDF.h>

namespace Photon {
    
    class Mirror : public BSDF {
    public:
        Mirror();
        Mirror(const Color& refl);

        Float evalPdf(const BSDFSample& sample) const;
        Color eval   (const BSDFSample& sample) const;
        Color sample (const Point2& rand, BSDFSample* sample) const;

    private:
        Color _refl;
    };

}