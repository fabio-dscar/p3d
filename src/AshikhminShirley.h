#pragma once

#include <BSDF.h>
#include <Microfacet.h>

namespace Photon {

    class AshikhminShirley : public BSDF {
    public:
        AshikhminShirley();
        AshikhminShirley(DistributionType dist, const Vec2& alpha, const Color& diff, const Color& spec);

        Float evalPdf(const BSDFSample& sample) const;
        Color eval(const BSDFSample& sample) const;
        Color sample(const Point2& rand, BSDFSample* sample) const;

    private:
        MicrofacetDist _dist;
        Color _diff;
        Color _spec;
    };

}