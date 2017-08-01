#include <StratifiedSampler.h>

#include <Sampling.h>

using namespace Photon;

StratifiedSampler::StratifiedSampler(uint32 nx, uint32 ny, uint32 numDims) : _nx(nx), _ny(ny), Sampler(nx * ny) {
    _numDims = numDims;
    for (uint32 d = 0; d < numDims; ++d) {
        _samples1D.allocArray(_spp);
        _samples2D.allocArray(_spp);
    }
}

Float StratifiedSampler::next1D() {
    if (_samples1D.hasNext())
        return _samples1D.next(_currSample);

    return _rng.uniform1D();
}

Point2 StratifiedSampler::next2D() {
    if (_samples2D.hasNext())
        return _samples2D.next(_currSample);

    return _rng.uniform2D();
}

void StratifiedSampler::nextND(uint32 N, std::vector<Float>& arr) const {
    arr.resize(N);
    for (uint32 i = 0; i < N; ++i)
        arr[i] = _rng.uniform1D();
}

void StratifiedSampler::start(const Point2ui& pixel) {
    _currPixel = pixel;
    _currSample = 0;
    _arrays1D.reset();
    _arrays2D.reset();
    _samples1D.reset();
    _samples2D.reset();

    for (uint32 d = 0; d < _numDims; ++d) {
        nRooks(_rng, _spp, 1, &_samples1D.arrays[d][0]);

        // Stratify and permute x and y coordinates
        multijittered2DArray(_rng, _nx, _ny, _samples2D.arrays[d]);
        //stratified2DArray(_rng, _nx, _ny, _samples2D.arrays[d]);
        //permute(_rng, _nx * _ny, 2, 0, &_samples2D.arrays[d][0][0]);
        //permute(_rng, _nx * _ny, 2, 1, &_samples2D.arrays[d][0][0]);
    }

    auto& arrays1D = _arrays1D.arrays;
    for (uint32 arr = 0; arr < arrays1D.size(); ++arr)
        nRooks(_rng, (uint32)arrays1D[arr].size(), 1, &arrays1D[arr][0]);

    auto& arrays2D = _arrays2D.arrays;
    for (uint32 arr = 0; arr < arrays2D.size(); ++arr)
        nRooks(_rng, (uint32)arrays2D[arr].size(), 2, &arrays2D[arr][0][0]);
}

void StratifiedSampler::startSample(uint32 sample) {
    _currSample = sample;
    _samples1D.reset();
    _samples2D.reset();
}

void StratifiedSampler::allocArray1D(uint32 numSamples) {
    _arrays1D.allocArray(numSamples);
}

void StratifiedSampler::allocArray2D(uint32 numSamples) {
    _arrays2D.allocArray(numSamples);
}

void StratifiedSampler::allocArray1D(std::vector<Float>& arr, uint32 numSamples) const {
    jittered1DArray(_rng, numSamples, arr);
}

void StratifiedSampler::allocArray2D(std::vector<Point2>& arr, uint32 numSamples) const {
    jittered2DArray(_rng, numSamples, arr);
}

const Float* StratifiedSampler::next1DArray(uint32 numSamples) {
    if (_arrays1D.hasNext())
        return &_arrays1D.nextArray(numSamples)[0];

    return nullptr;
}

const Point2* StratifiedSampler::next2DArray(uint32 numSamples) {
    if (_arrays2D.hasNext())
        return &_arrays2D.nextArray(numSamples)[0];

    return nullptr;
}

std::unique_ptr<Sampler> StratifiedSampler::copy(uint32 seed) const {
    std::unique_ptr<Sampler> samp = std::make_unique<StratifiedSampler>(*this);
    samp->setSeq(seed);
    return std::move(samp);
}