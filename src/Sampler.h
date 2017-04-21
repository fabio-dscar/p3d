#pragma once

#include <PhotonMath.h>
#include <Vector.h>
#include <Random.h>

namespace Photon {

    class Sampler {
    public:
        Sampler() : _rng() { }

        virtual void start(const Point2ui& pixel) const = 0;

        virtual Float  next1D() const = 0;
        virtual Point2 next2D() const = 0;      
        virtual std::unique_ptr<Float> nextND(uint32 N) const = 0;

        virtual std::vector<Float>&  next1DArray() const = 0;
        virtual std::vector<Point2>& next2DArray() const = 0;

        virtual std::unique_ptr<Sampler> copy(uint32 seed) const = 0;

    protected:
        RandGen _rng;
    };

}