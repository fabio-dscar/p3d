#include <Distribution.h>

using namespace Photon;

DiscretePdf1D::DiscretePdf1D(const std::vector<Float>& vals) {
    uint32 size = (uint32)vals.size();

    // Copy the function
    _f.resize(size);
    for (uint32 idx = 0; idx < size; ++idx)
        _f[idx] = vals[idx];

    // Build CDF
    _cdf.resize(size + 1);

    _cdf[0] = 0;
    for (uint32 idx = 1; idx < (size + 1); ++idx)
        _cdf[idx] += _cdf[idx - 1] + _f[idx - 1] / size;

    // Normalize CDF
    _sum = _cdf[size];
    for (uint32 idx = 1; idx < (size + 1); ++idx)
        _cdf[idx] /= _sum;
}

Float DiscretePdf1D::pdf(uint32 idx) const {
    return _f[idx] / (_f.size() * _sum);
}

uint32 DiscretePdf1D::cdf(uint32 idx) const {
    return _cdf[idx];
}

Float DiscretePdf1D::operator()(uint32 idx) const {
    return _f[idx];
}

uint32 DiscretePdf1D::sample(Float rand) const {
    auto it = std::upper_bound(_cdf.begin(), _cdf.end(), rand);
    return std::distance(_cdf.begin(), it) - 1;
}