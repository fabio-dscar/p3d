#include <Random.h>

using namespace Photon;

void Photon::permute(const RandGen& rng, uint32 numSamples, uint32 numDims, uint32 dim, Float* arr) {
    for (uint32 s = 0; s < numSamples; ++s) {
        uint32 idx = s + rng.uniformUInt32(numSamples - s);
        std::swap(arr[dim + numDims * s],
                  arr[dim + numDims * idx]);
    }
}

void Photon::nRooks(const RandGen& rng, uint32 numSamples, uint32 numDims, Float* arr) {
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

void Photon::stratified2DArray(const RandGen& rng, uint32 nx, uint32 ny, std::vector<Point2>& arr) {
    arr.resize(nx * ny);

    for (uint32 x = 0; x < nx; ++x) {
        for (uint32 y = 0; y < ny; ++y) {
            Float dx = ((Float)x + rng.uniform1D()) / nx;
            Float dy = ((Float)y + rng.uniform1D()) / ny;

            arr[x + nx * y] = Point2(dx, dy);
        }
    }
}

void Photon::multijittered2DArray(const RandGen& rng, uint32 nx, uint32 ny, std::vector<Point2>& arr) {
    arr.resize(nx * ny);

    // Build canonical multijittered arrangement
    for (uint32 x = 0; x < nx; ++x) {
        for (uint32 y = 0; y < ny; ++y) {
            Float dx = ((Float)y + ((Float)x + rng.uniform1D()) / (Float)nx) / (Float)ny;
            Float dy = ((Float)x + ((Float)y + rng.uniform1D()) / (Float)ny) / (Float)nx;

            arr[x + nx * y] = Point2(dx, dy);
        }
    }

    permute(rng, nx * ny, 2, 0, &arr[0][0]);
    permute(rng, nx * ny, 2, 1, &arr[0][0]); 
}

void Photon::jittered2DArray(const RandGen& rng, uint32 numSamples, std::vector<Point2>& arr) {
    arr.resize(numSamples);
    nRooks(rng, numSamples, 2, &arr[0][0]);
}

void Photon::jittered1DArray(const RandGen& rng, uint32 numSamples, std::vector<Float>& arr) {
    arr.resize(numSamples);
    nRooks(rng, numSamples, 1, &arr[0]);
}

RandGen::RandGen(uint64 seq) : _state(0xBA5EBA11), _seq(seq) {}
RandGen::RandGen(uint64 state, uint64 seq)
    : _state(state), _seq(seq) {}

void RandGen::setSeq(uint64 seq) {
    _seq = seq;
}

uint32 RandGen::uniformUInt32() const {
    uint64 oldstate = _state;
    _state = oldstate * 6364136223846793005ULL + (_seq | 1);
    uint32 xorshifted = (uint32)(((oldstate >> 18u) ^ oldstate) >> 27u);
    uint32 rot = (uint32)(oldstate >> 59u);
    return (xorshifted >> rot) | (xorshifted << ((~rot + 1u) & 31));
}

uint32 RandGen::uniformUInt32(uint32 max) const {
    return uniformUInt32() % max;
}

Float RandGen::uniform1D() const {
    return std::min(ONE_MINUS_EPSILON, Float(uniformUInt32() * 2.3283064365386963e-10f));
}

Point2 RandGen::uniform2D() const {
    return Point2(uniform1D(), uniform1D());
}

