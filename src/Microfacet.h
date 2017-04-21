#pragma once

#include <BSDF.h>

namespace Photon {

    enum DistributionType {
        PHONG = 0,
        BECKMANN = 1,
        GGX = 2
    };

    /*
    class MicrofacetDist {
    public:
        MicrofacetDist() { }

        virtual Float evalD(const Vec3& wh) const = 0;
        virtual Float evalMask(const Vec3& wi, const Vec3& wo) const = 0;
    };

*/    



    class MicrofacetBSDF : public BSDF {
    public:
        MicrofacetBSDF(BSDFType type, DistributionType dist) : BSDF(type), _dist(dist) { }

        Float evalPdf(const BSDFSample& sample) const {

        }

        Color eval(const BSDFSample& sample) const {

        }

        Color sample(const Point2& rand, BSDFSample* sample) const {

        }

    private:
        DistributionType _dist;
    };

}