#pragma once

#include <Sampler.h>

namespace Photon {

    class RandomSampler : public Sampler {
    public:
        RandomSampler(uint32 spp, uint32 numDims) : Sampler(spp) {
            _numDims = numDims;
        }

        void start(const Point2ui& pixel) {
            _currSample = 0;

            /*for (uint32 d = 0; d < _numDims; ++d) {
                for (uint32 p = 0; p < _spp; ++p) {
                    jittered1DArray(_rng, _spp, _samples1D.arrays[d]);
                    jittered2DArray(_rng, _spp, _samples2D.arrays[d]);
                }
            }

            auto arrays1D = _arrays1D.arrays;
            for (uint32 arr = 0; arr < arrays1D.size() * _spp; ++arr)
                jittered1DArray(_rng, (uint32)arrays1D[arr].size(), arrays1D[arr]);

            auto arrays2D = _arrays2D.arrays;
            for (uint32 arr = 0; arr < arrays2D.size() * _spp; ++arr)
                jittered2DArray(_rng, (uint32)arrays2D[arr].size(), arrays2D[arr]);*/
        }

        void startSample(uint32 sample) {
            _currSample = sample;
            
        }

        void allocArray1D(uint32 numSamples) {

        }

        void allocArray2D(uint32 numSamples) {

        }

        void allocArray1D(std::vector<Float>& arr, uint32 numSamples) const {
            arr.resize(numSamples);
            for (uint32 s = 0; s < numSamples; ++s)
                arr[s] = _rng.uniform1D();
        }

        void allocArray2D(std::vector<Point2>& arr, uint32 numSamples) const {
            arr.resize(numSamples);
            for (uint32 s = 0; s < numSamples; ++s)
                arr[s] = _rng.uniform2D();
        }

        Float next1D() {
            return _rng.uniform1D();
        }

        Point2 next2D() {
            return _rng.uniform2D();
        }

        void nextND(uint32 N, std::vector<Float>& arr) const {
            for (uint32 n = 0; n < N; ++n)
                arr[n] = _rng.uniform1D();
        }

        const Float*  next1DArray(uint32 numSamples) {
            return nullptr;
        }

        const Point2* next2DArray(uint32 numSamples) {
            return nullptr;
        }

        std::unique_ptr<Sampler> copy(uint32 seed) const {
            std::unique_ptr<Sampler> samp = std::make_unique<RandomSampler>(*this);
            samp->setSeq(seed);
            return std::move(samp);
        }

    private:
        SampleArrays<Float>  _samples1D;
        SampleArrays<Point2> _samples2D;

        SampleArrays<Float>  _arrays1D;
        SampleArrays<Point2> _arrays2D;
    };

}