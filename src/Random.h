#pragma once

#include <cmath>

#include <PhotonMath.h>
#include <Vector.h>

namespace Photon {

    static const double D_ONE_MINUS_EPSILON = 0.99999999999999989;
    static const float  F_ONE_MINUS_EPSILON = 0.99999994f;

#if PHOTON_DOUBLE
    static const Float ONE_MINUS_EPSILON = D_ONE_MINUS_EPSILON;
#else
    static const Float ONE_MINUS_EPSILON = F_ONE_MINUS_EPSILON;
#endif

    class RandGen {
    public:
        RandGen(uint64 seq = 0);
        RandGen(uint64 state, uint64 seq);

        void setSeq(uint64 seq);

        uint32 uniformUInt32() const;
        uint32 uniformUInt32(uint32 max) const;

        Float  uniform1D() const;
        Point2 uniform2D() const;

    private:
        mutable uint64 _state;
        uint64 _seq;
    };

    void multijittered2DArray(const RandGen& rng, uint32 nx, uint32 ny, std::vector<Point2>& arr);
    void jittered2DArray(const RandGen& rng, uint32 numSamples, std::vector<Point2>& arr);
    void jittered1DArray(const RandGen& rng, uint32 numSamples, std::vector<Float>& arr);

    void stratified2DArray(const RandGen& rng, uint32 nx, uint32 ny, std::vector<Point2>& arr);
    void nRooks(const RandGen& rng, uint32 numSamples, uint32 numDims, Float* arr);

    void permute(const RandGen& rng, uint32 numSamples, uint32 numDims, uint32 dim, Float* arr);
}