#pragma once

#define PHOTON_DOUBLE 1

#include <PhotonTracer.h>
#include <IntTypes.h>

namespace Photon {
    
#if PHOTON_DOUBLE
    typedef double Float;

    static const Float F_EPSILON = 1e-10;
    static const Float F_RAY_OFFSET = 1e-6;
#else
    typedef float Float;

    static const Float F_EPSILON = 1e-6f;
    static const Float F_RAY_OFFSET = 1e-3f;
#endif

    // Math constants
    static PHOTON_CONSTEXPR Float PI        = 3.14159265358979323846;
    static PHOTON_CONSTEXPR Float INVPI     = 0.31830988618379067154;
    static PHOTON_CONSTEXPR Float INV2PI    = 0.15915494309189533577;
    static PHOTON_CONSTEXPR Float INV4PI    = 0.07957747154594766788;
    static PHOTON_CONSTEXPR Float PIOVER2   = 1.57079632679489661923;
    static PHOTON_CONSTEXPR Float PIOVER4   = 0.78539816339744830961;
    static PHOTON_CONSTEXPR Float SQRTINVPI = 0.56418958354775628694807945156077;
    static PHOTON_CONSTEXPR Float SQRT2     = 1.41421356237309504880;
    static PHOTON_CONSTEXPR Float INVLOG2   = 1.442695040888963387004650940071;

    static PHOTON_CONSTEXPR Float F_LOWEST   = std::numeric_limits<Float>::lowest();
    static PHOTON_CONSTEXPR Float F_MAXIMUM  = std::numeric_limits<Float>::max();
    static PHOTON_CONSTEXPR Float F_INFINITY = std::numeric_limits<Float>::infinity();

    // Math utility functions
    namespace Math {
        template<typename T, typename U, typename V>
        inline T clamp(T val, U low, V high);

        template<typename T>
        inline T clamp(T val, T low, T high);

        inline Float max(Float x, Float y);
        inline Float min(Float x, Float y);
        inline Float acosSafe(Float x);
        inline Float sqrtSafe(Float x);
        inline Float radians (Float degrees);
        inline Float degrees (Float radians);
        inline Float log2    (Float x);

        inline Float erf(Float x);
        inline Float erfInv(Float x);

        inline int32 sign(Float scalar);
        inline Float lerp(Float t, Float v1, Float v2);

        bool solQuadratic(Float a, Float b, Float c, Float* x0, Float* x1);
        bool solSystem2x2(const Float A[2][2], const Float b[2], Float* x0, Float* x1);
    }

}

#include <PhotonMath.inl>