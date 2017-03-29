#pragma once

#define PHOTON_USE_GLM 0
#define PHOTON_USE_DOUBLE 1

#include <PhotonTracer.h>
#include <IntTypes.h>

namespace Photon {

#if PHOTON_USE_DOUBLE
    typedef double Float;

    static const Float F_EPSILON = 1e-10;
    static const Float F_RAY_OFFSET = 1e-6;
#else
    typedef float Float;

    static const Float F_EPSILON = 1e-6f;
    static const Float F_RAY_OFFSET = 1e-3f;
#endif

    // Math constants
    static PHOTON_CONSTEXPR Float PI      = 3.14159265358979323846;
    static PHOTON_CONSTEXPR Float INVPI   = 0.31830988618379067154;
    static PHOTON_CONSTEXPR Float INV2PI  = 0.15915494309189533577;
    static PHOTON_CONSTEXPR Float INV4PI  = 0.07957747154594766788;
    static PHOTON_CONSTEXPR Float PIOVER2 = 1.57079632679489661923;
    static PHOTON_CONSTEXPR Float PIOVER4 = 0.78539816339744830961;
    static PHOTON_CONSTEXPR Float SQRT2   = 1.41421356237309504880;
    static PHOTON_CONSTEXPR Float INVLOG2 = 1.442695040888963387004650940071;

    static PHOTON_CONSTEXPR Float F_LOWEST   = std::numeric_limits<Float>::lowest();
    static PHOTON_CONSTEXPR Float F_MAXIMUM  = std::numeric_limits<Float>::max();
    static PHOTON_CONSTEXPR Float F_INFINITY = std::numeric_limits<Float>::infinity();

    namespace Math {
        template<typename T, typename U, typename V>
        inline T clamp(T val, U low, V high);

        template<typename T>
        inline T clamp(T val, T low, T high);

        inline Float radians(Float degrees);
        inline Float degrees(Float radians);
        inline Float log2(Float x);

        inline int32 sign(Float scalar);
        inline Float lerp(Float t, Float v1, Float v2);

        bool solveQuadratic(Float a, Float b, Float c, Float* x0, Float* x1);
        bool solveLinearSystem2x2(const Float A[2][2], const Float b[2], Float* x0, Float* x1);
    }

}

// Implementation
namespace Photon {
namespace Math {

    template<typename T, typename U, typename V>
    inline T clamp(T val, U low, V high) {
        if (val < low)
            return low;
        else if (val > high)
            return high;
        else
            return val;
    }

    template<typename T>
    inline T clamp(T val, T low, T high) {
        return clamp<T, T, T>(val, low, high);
    }

    inline int32 sign(Float scalar) {
        if (scalar < 0)
            return -1;

        return 1;
    }

    inline Float radians(Float degrees) {
        return (PI / 180) * degrees;
    }

    inline Float degrees(Float radians) {
        return (180 / PI) * radians;
    }

    inline Float log2(Float x) {
        return std::log(x) * INVLOG2;
    }

    inline Float lerp(Float t, Float v1, Float v2) {
        return (1 - t) * v1 + t * v2;
    }

    inline bool solveQuadratic(Float a, Float b, Float c, Float* x0, Float* x1) {
        double disc = b * b - 4 * a * c;
        if (disc < 0)
            return false;

        double rootDisc = std::sqrt(disc);
        double q;
        if (b < 0)
            q = -0.5 * (b - rootDisc);
        else
            q = -0.5 * (b + rootDisc);

        *x0 = q / a;
        *x1 = c / q;

        if (*x0 > *x1)
            std::swap(x0, x1);

        return true;
    }

    inline bool solveLinearSystem2x2(const Float A[2][2], const Float b[2], Float* x0, Float* x1) {
        Float det = A[0][0] * A[1][1] - A[0][1] * A[1][0];
        if (std::abs(det) < 1e-10f)
            return false;

        *x0 = (A[1][1] * b[0] - A[0][1] * b[1]) / det;
        *x1 = (A[0][0] * b[1] - A[1][0] * b[0]) / det;

        if (std::isnan(*x0) || std::isnan(*x1))
            return false;

        return true;
    }

}
}

/*template<typename T, typename U, typename V>
inline T Photon::Math::clamp(T val, U low, V high) {
    if (val < low)
        return low;
    else if (val > high)
        return high;
    else
        return val;
}*/