#include <RandomSampler.h>

using namespace Photon;

RandomSampler::RandomSampler(uint32 spp, uint32 numDims) : Sampler(spp) {
    _numDims = numDims;
}

void RandomSampler::start(const Point2ui& pixel) {
    _currPixel = pixel;
    _currSample = 0;
}

void RandomSampler::startSample(uint32 sample) {
    _currSample = sample;
}

void RandomSampler::allocArray1D(uint32 numSamples) {

}

void RandomSampler::allocArray2D(uint32 numSamples) {

}

void RandomSampler::allocArray1D(std::vector<Float>& arr, uint32 numSamples) const {
    arr.resize(numSamples);
    for (uint32 s = 0; s < numSamples; ++s)
        arr[s] = _rng.uniform1D();
}

void RandomSampler::allocArray2D(std::vector<Point2>& arr, uint32 numSamples) const {
    arr.resize(numSamples);
    for (uint32 s = 0; s < numSamples; ++s)
        arr[s] = _rng.uniform2D();
}

Float RandomSampler::next1D() {
    return _rng.uniform1D();
}

Point2 RandomSampler::next2D() {
    return _rng.uniform2D();
}

void RandomSampler::nextND(uint32 N, std::vector<Float>& arr) const {
    for (uint32 n = 0; n < N; ++n)
        arr[n] = _rng.uniform1D();
}

const Float*  RandomSampler::next1DArray(uint32 numSamples) {
    return nullptr;
}

const Point2* RandomSampler::next2DArray(uint32 numSamples) {
    return nullptr;
}

std::unique_ptr<Sampler> RandomSampler::copy(uint32 seed) const {
    std::unique_ptr<Sampler> samp = std::make_unique<RandomSampler>(*this);
    samp->setSeq(seed);
    return std::move(samp);
}
