#pragma once

#include <cmath>

#include <PhotonMath.h>

namespace Photon {

    static const double D_ONE_MINUS_EPSILON = 0.99999999999999989;
    static const float F_ONE_MINUS_EPSILON = 0.99999994;

#if PHOTON_USE_DOUBLE
    static const Float ONE_MINUS_EPSILON = D_ONE_MINUS_EPSILON;
#else
    static const Float ONE_MINUS_EPSILON = F_ONE_MINUS_EPSILON;
#endif

    class RandGen {
    public:
        RandGen() : _state(0xBA5EBA11), _seq(0) { }
        RandGen(uint64 state, uint64 seq = 0) 
            : _state(state), _seq(seq) { }

        inline uint32 uniformUInt32() const {
            uint64 oldstate = _state;
            _state = oldstate * 6364136223846793005ULL + (_seq | 1);
            uint32 xorshifted = (uint32)(((oldstate >> 18u) ^ oldstate) >> 27u);
            uint32 rot = (uint32)(oldstate >> 59u);
            return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
        }

        inline Float uniformFloat() const {
            return std::min(ONE_MINUS_EPSILON, Float(uniformUInt32() * 2.3283064365386963e-10f));
        }

        inline void jittered2DArray(uint32 nx, uint32 ny, std::vector<Point2>& arr, bool yes) const {
            //std::vector<Point2> arr(nx * ny);

            if (yes) {
                for (uint32 x = 0; x < nx; ++x) {
                    for (uint32 y = 0; y < ny; ++y) {
                        Float dx = ((Float)x + uniformFloat()) / nx;
                        Float dy = ((Float)y + uniformFloat()) / ny;

                        arr.emplace_back(dx, dy);
                    }
                }
            } else {
                for (uint32 x = 0; x < nx; ++x) {
                    for (uint32 y = 0; y < ny; ++y) {
                        Float dx = uniformFloat();
                        Float dy = uniformFloat();

                        arr.emplace_back(dx, dy);
                    }
                }
            }
        }

    private:
        mutable uint64 _state;
        uint64 _seq;
    };

}