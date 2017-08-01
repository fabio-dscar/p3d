#pragma once

#include <vector>

#include <PhotonMath.h>

namespace Photon {

    class DiscretePdf1D {
    public:
        DiscretePdf1D(const std::vector<Float>& vals);

        Float  pdf(uint32 idx) const;
        uint32 cdf(uint32 idx) const;
        uint32 sample(Float rand) const;

        Float operator()(uint32 idx) const;

    private:
        std::vector<Float> _f;
        std::vector<Float> _cdf;
        Float _sum;
    };


    class ContinuousPdf2D {
    public:

    private:

    };

}