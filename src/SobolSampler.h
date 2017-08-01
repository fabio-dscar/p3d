#pragma once

#include <Sampler.h>

namespace Photon {

    class SobolSampler : public Sampler {
    public:
        SobolSampler(uint32 spp, uint32 numDims = 0) : Sampler(spp) {

        }

        void start(const Point2ui& pixel) = 0;
        void startSample(uint32 sample) = 0;

        void allocArray1D(uint32 numSamples) = 0;
        void allocArray2D(uint32 numSamples) = 0;

        void allocArray1D(std::vector<Float>& arr, uint32 numSamples) const = 0;
        void allocArray2D(std::vector<Point2>& arr, uint32 numSamples) const = 0;

        Float  next1D() = 0;
        Point2 next2D() = 0;
        void nextND(uint32 N, std::vector<Float>& arr) const = 0;

        const Float*  next1DArray(uint32 numSamples) = 0;
        const Point2* next2DArray(uint32 numSamples) = 0;

        std::unique_ptr<Sampler> copy(uint32 seed) const = 0;
    };

}