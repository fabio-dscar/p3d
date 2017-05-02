#pragma once

#include <PhotonMath.h>
#include <Vector.h>
#include <Random.h>

namespace Photon {

    template<typename T>
    struct SampleArray {
        uint32 size;
        std::vector<T> array;
    };

    class Sampler {
    public:
        Sampler(uint32 spp) : _spp(spp), _rng() { }

        virtual void start(const Point2ui& pixel) const = 0;

        void allocArray1D(uint32 numSamples) {
            SampleArray<Float> arr;
            arr.size = numSamples;
            arr.array.resize(numSamples);

            _arrays1D.push_back(arr);
        }

        void allocArray2D(uint32 numSamples) {
            SampleArray<Point2> arr;
            arr.size = numSamples;
            arr.array.resize(numSamples);

            _arrays2D.push_back(arr);
        }

        virtual Float  next1D() = 0;
        virtual Point2 next2D() = 0;      
        virtual std::unique_ptr<Float> nextND(uint32 N) const = 0;

        virtual const SampleArray<Float>&  next1DArray() = 0;
        virtual const SampleArray<Point2>& next2DArray() = 0;

        virtual std::unique_ptr<Sampler> copy(uint32 seed) const = 0;

        uint32 spp() const {
            return _spp;
        }
    protected:
        RandGen _rng;

        uint32 _spp;
        uint32 _numDims;

        uint32 _currSample;

        uint32 _currDim1D;
        uint32 _currDim2D;

        uint32 _currArr1D;
        uint32 _currArr2D;

        std::vector<SampleArray<Float>>  _arrays1D;
        std::vector<SampleArray<Point2>> _arrays2D;
    
        std::vector<SampleArray<Float>>  _samples1D;
        std::vector<SampleArray<Point2>> _samples2D;
    };

}