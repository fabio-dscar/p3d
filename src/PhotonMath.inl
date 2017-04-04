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