#pragma once

#include <cstdint>

namespace Photon {

    // Integer types
    typedef std::uint8_t  uint8;
    typedef std::uint16_t uint16;
    typedef std::uint32_t uint32;
    typedef std::uint64_t uint64;

    typedef std::int8_t  int8;
    typedef std::int16_t int16;
    typedef std::int32_t int32;
    typedef std::int64_t int64;

    // Math constants
    static const float F_EPSILON = 1e-6f;
    static const float F_RAY_OFFSET = 1e-2f;

    static const float F_INFINITY = std::numeric_limits<float>::infinity();


}