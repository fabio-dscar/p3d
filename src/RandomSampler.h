#pragma once

#include <Sampler.h>

namespace Photon {

    class RandomSampler : public Sampler {
    public:
        RandomSampler(uint32 spp, uint32 numDims = 0);

        void start(const Point2ui& pixel);
        void startSample(uint32 sample);

        void allocArray1D(uint32 numSamples);
        void allocArray2D(uint32 numSamples);

        void allocArray1D(std::vector<Float>& arr, uint32 numSamples) const;
        void allocArray2D(std::vector<Point2>& arr, uint32 numSamples) const;

        Float next1D();
        Point2 next2D();
        void nextND(uint32 N, std::vector<Float>& arr) const;

        const Float*  next1DArray(uint32 numSamples);
        const Point2* next2DArray(uint32 numSamples);

        std::unique_ptr<Sampler> copy(uint32 seed) const;
    };

}