#pragma once

#include <cmath>

#include <PhotonMath.h>

namespace Photon {

    static const double D_ONE_MINUS_EPSILON = 0.99999999999999989;
    static const float F_ONE_MINUS_EPSILON = 0.99999994;

#if PHOTON_DOUBLE
    static const Float ONE_MINUS_EPSILON = D_ONE_MINUS_EPSILON;
#else
    static const Float ONE_MINUS_EPSILON = F_ONE_MINUS_EPSILON;
#endif

    class RandGen {
    public:
        RandGen(uint64 seq = 0) : _state(0xBA5EBA11), _seq(seq) {}
        RandGen(uint64 state, uint64 seq) 
            : _state(state), _seq(seq) { }

        void setSeq(uint64 seq) {
            _seq = seq;
        }

        inline uint32 uniformUInt32() const {
            uint64 oldstate = _state;
            _state = oldstate * 6364136223846793005ULL + (_seq | 1);
            uint32 xorshifted = (uint32)(((oldstate >> 18u) ^ oldstate) >> 27u);
            uint32 rot = (uint32)(oldstate >> 59u);
            return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
        }

        inline uint32 uniformUInt32(uint32 max) const {
            return uniformUInt32() % max;
        }

        inline Float uniform1D() const {
            return std::min(ONE_MINUS_EPSILON, Float(uniformUInt32() * 2.3283064365386963e-10f));
        }

        inline Point2 uniform2D() const {
            return Point2(uniform1D(), uniform1D());
        }

    private:
        mutable uint64 _state;
        uint64 _seq;
    };

    inline void permute(const RandGen& rng, uint32 numSamples, uint32 numDims, uint32 dim, Float* arr) {
        for (uint32 s = 0; s < numSamples; ++s) {
            uint32 idx = s + rng.uniformUInt32(numSamples - s);
            std::swap(arr[dim + numDims * s], 
                      arr[dim + numDims * idx]);
        }
    }

    inline void nRooks(const RandGen& rng, uint32 numSamples, uint32 numDims, Float* arr) {  
        Float invNumSamples = 1.0 / numSamples;
        for (uint32 s = 0; s < numSamples; ++s) {
            for (uint32 d = 0; d < numDims; ++d) {
                Float u = rng.uniform1D();
                Float samp = (s + u) * invNumSamples;
                arr[d + numDims * s] = samp;
            }
        }

        for (uint32 d = 0; d < numDims; ++d)
            permute(rng, numSamples, numDims, d, arr);
    }
    
    inline void stratified2DArray(const RandGen& rng, uint32 nx, uint32 ny, std::vector<Point2>& arr) {
        arr.resize(nx * ny);

        for (uint32 x = 0; x < nx; ++x) {
            for (uint32 y = 0; y < ny; ++y) {
                Float dx = ((Float)x + rng.uniform1D()) / nx;
                Float dy = ((Float)y + rng.uniform1D()) / ny;

                arr[x + nx * y] = Point2(dx, dy);
            }
        }
    }

    inline void multijittered2DArray(const RandGen& rng, uint32 nx, uint32 ny, std::vector<Point2>& arr) {
        arr.resize(nx * ny);

        // Build multijittered arrangement
        for (uint32 x = 0; x < nx; ++x) {
            for (uint32 y = 0; y < ny; ++y) {
                Float dx = (y + ((Float)x + rng.uniform1D()) / nx) / ny;
                Float dy = (x + ((Float)y + rng.uniform1D()) / ny) / nx;

                arr[x * ny + y] = Point2(dx, dy);
            }
        }

        // Shuffle the arrangement in the x
        for (uint32 x = 0; x < nx; ++x) {
            uint32 k = x + rng.uniformUInt32(nx - x);
            for (uint32 y = 0; y < ny; ++y)
                std::swap(arr[x * ny + y].x,
                          arr[k * ny + y].x);
        }

        // Shuffle the arrangement in the y
        for (uint32 y = 0; y < ny; ++y) {
            uint32 k = y + rng.uniformUInt32(ny - y);
            for (uint32 x = 0; x < nx; ++x) {
                std::swap(arr[x * ny + y].y,
                          arr[x * ny + k].y);
            }
        }
    }

    inline void jittered2DArray(const RandGen& rng, uint32 numSamples, std::vector<Point2>& arr) {
        arr.resize(numSamples);
        nRooks(rng, numSamples, 2, &arr[0][0]);
    }
    
    inline void jittered1DArray(const RandGen& rng, uint32 numSamples, std::vector<Float>& arr) {
        arr.resize(numSamples);
        nRooks(rng, numSamples, 1, &arr[0]);
    }
}