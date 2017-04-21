#pragma once

#include <Sampler.h>

namespace Photon {

    class RandomSampler : public Sampler {
    public:
        RandomSampler() : Sampler() { }

        Float next1D() const {
            return _rng.uniformFloat();
        }

        Point2 next2D() const {
            return Point2(_rng.uniformFloat(), _rng.uniformFloat());
        }

        std::unique_ptr<Float> nextND(uint32 n) const {
            Float* ptr = new Float[n];
            for (uint32 i = 0; i < n; ++i)
                ptr[i] = _rng.uniformFloat();

            return std::unique_ptr<Float>(ptr);
        }

        void start(const Point2ui& pixel) const {

        }

        std::vector<Float>  next1DArray() const {
            
        }

        std::vector<Point2> next2DArray() const {

        }

        std::unique_ptr<Sampler> copy(uint32 seed) const {

        }

    private:

    };

}