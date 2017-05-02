#pragma once

#include <Sampler.h>

namespace Photon {

    class RandomSampler : public Sampler {
    public:
        RandomSampler(uint32 spp, uint32 numDims) : Sampler(spp) { 
            
            _samples1D.resize(numDims);
            for (uint32 d = 0; d < numDims; ++d) {
                _samples1D[d].size = spp;
                _samples1D[d].array.resize(spp);
            }

            _samples2D.resize(numDims);
            for (uint32 d = 0; d < numDims; ++d) {
                _samples2D[d].size = spp;
                _samples2D[d].array.resize(spp);
            }

        }

        Float next1D() {
            if (_samples1D.size() <= _currDim1D)
                return _rng.uniformFloat();

            return _samples1D[_currDim1D++].array[_currSample];
        }

        Point2 next2D() {
            if (_samples2D.size() <= _currDim2D)
                return _rng.uniform2D();

            return _samples2D[_currDim2D++].array[_currSample];
        }

        std::unique_ptr<Float> nextND(uint32 n) const {
            Float* ptr = new Float[n];
            for (uint32 i = 0; i < n; ++i)
                ptr[i] = _rng.uniformFloat();

            return std::unique_ptr<Float>(ptr);
        }

        void start(const Point2ui& pixel) {
            for (uint32 d = 0; d < _numDims; ++d)
                for (uint32 p = 0; p < _spp; ++p)
                    _samples1D[d].array[p] = _rng.uniformFloat();

            for (uint32 d = 0; d < _numDims; ++d)
                for (uint32 p = 0; p < _spp; ++p)
                    _samples2D[d].array[p] = Point2(_rng.uniform2D());

            for (uint32 arr = 0; arr < _arrays1D.size(); ++arr)
                for (uint32 s = 0; s < _arrays1D[arr].size; ++s)
                    _arrays1D[arr].array[s] = _rng.uniformFloat();

            for (uint32 arr = 0; arr < _arrays2D.size(); ++arr)
                for (uint32 s = 0; s < _arrays2D[arr].size; ++s)
                    _arrays2D[arr].array[s] = Point2(_rng.uniform2D());
        }

        const SampleArray<Float>& next1DArray() {
            if (_arrays1D.size() <= _currArr1D)
                ;// return _rng.uniform2D();

            return _arrays1D[_currArr1D++];
        }

        const SampleArray<Point2>& next2DArray() {
            if (_arrays2D.size() <= _currArr2D)
                ;// return _rng.uniform2D();

            return _arrays2D[_currArr2D++];
        }

        std::unique_ptr<Sampler> copy(uint32 seed) const {

        }

    private:

    };

}