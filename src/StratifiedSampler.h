#pragma once

#include <Sampler.h>

namespace Photon {

    class StratifiedSampler : public Sampler {
    public:
        StratifiedSampler(uint32 nx, uint32 ny, uint32 numDims);

        Float  next1D();
        Point2 next2D();
        void nextND(uint32 N, std::vector<Float>& arr) const;

        void start(const Point2ui& pixel);
        void startSample(uint32 sample);

        void allocArray1D(uint32 numSamples);
        void allocArray2D(uint32 numSamples);

        void allocArray1D(std::vector<Float>& arr, uint32 numSamples) const;
        void allocArray2D(std::vector<Point2>& arr, uint32 numSamples) const;

        const Float*  next1DArray(uint32 numSamples);
        const Point2* next2DArray(uint32 numSamples);

        std::unique_ptr<Sampler> copy(uint32 seed) const;

    private:
        uint32 _nx, _ny;

        SampleArrays<Float>  _samples1D;
        SampleArrays<Point2> _samples2D;

        SampleArrays<Float>  _arrays1D;
        SampleArrays<Point2> _arrays2D;
    };

}