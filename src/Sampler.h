#pragma once

#include <PhotonMath.h>
#include <Vector.h>
#include <Random.h>

#include <vector>

namespace Photon {

    template<typename T>
    struct SampleArrays;

    class Sampler {
    public:
        Sampler(uint32 spp) : _spp(spp), _rng() { }
        Sampler(uint64 seq, uint32 spp) : _spp(spp), _rng(seq) {}
        Sampler(const Sampler& sampler, uint64 seq) 
            : Sampler(sampler) { 
            
            _rng.setSeq(seq);
        }

        void setSeq(uint64 seq) {
            _rng.setSeq(seq);
        }

        virtual void start(const Point2ui& pixel) = 0;
        virtual void startSample(uint32 sample) = 0;

        virtual void allocArray1D(uint32 numSamples) = 0;
        virtual void allocArray2D(uint32 numSamples) = 0;

        virtual void allocArray1D(std::vector<Float>& arr, uint32 numSamples) const = 0;
        virtual void allocArray2D(std::vector<Point2>& arr, uint32 numSamples) const = 0;

        virtual Float  next1D() = 0;
        virtual Point2 next2D() = 0;      
        virtual void nextND(uint32 N, std::vector<Float>& arr) const = 0;

        virtual const Float*  next1DArray(uint32 numSamples) = 0;
        virtual const Point2* next2DArray(uint32 numSamples) = 0;

        virtual std::unique_ptr<Sampler> copy(uint32 seed) const = 0;

        uint32 spp() const {
            return _spp;
        }

        Point2ui pixel() const {
            return _currPixel;
        }

    protected:
        RandGen _rng;

        uint32 _spp;
        uint32 _numDims;

        uint32 _currSample;
        Point2ui _currPixel;
    };

}

#include <Sampler.inl>