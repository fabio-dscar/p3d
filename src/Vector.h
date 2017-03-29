#pragma once

#if PHOTON_USE_GLM

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>

#endif

#include <MathDefs.h>

namespace Photon {

namespace Math {

    // Forward declaration
    template<typename T> class Normal3;

    template<typename T>
    class Vector3 {
    public:
        T x, y, z;

        Vector3() : x(0), y(0), z(0) {}
        Vector3(T scalar) : x(scalar), y(scalar), z(scalar) {}
        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
        Vector3(const Normal3<T>& normal);

        template<typename U>
        Vector3(const Vector3<U>& vec) {
            x = T(vec.x);
            y = T(vec.y);
            z = T(vec.z);
        }

        Vector3<T> operator+(const Vector3<T>& vec) const {
            return Vector3(x + vec.x, y + vec.y, z + vec.z);
        }

        Vector3<T>& operator+=(const Vector3<T>& vec) {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        Vector3<T> operator-(const Vector3<T>& vec) const {
            return Vector3(x - vec.x, y - vec.y, z - vec.z);
        }

        Vector3<T>& operator-=(const Vector3<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        Vector3<T> operator*(const Vector3<T>& vec) const {
            return Vector3(x * vec.x, y * vec.y, z * vec.z);
        }

        Vector3<T> operator*(T scalar) const {
            return Vector3(x * scalar, y * scalar, z * scalar);
        }

        Vector3<T>& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Vector3<T> operator/(T scalar) const {
            T recip = 1 / scalar;
            return Vector3(x * recip, y * recip, z * recip);
        }

        Vector3<T>& operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            z *= recip;
            return *this;
        }

        Vector3<T> operator-() const {
            return Vector3<T>(-x, -y, -z);
        }

        Float lengthSqr() const {
            return x * x + y * y + z * z;
        }

        Float length() const {
            return std::sqrt(lengthSqr());
        }

        void normalize() {
            *this /= length();
        }

        T min() const {
            return std::min(x, std::min(y, z));
        }

        T max() const {
            return std::max(x, std::max(y, z));
        }

        uint32 maxDim() const {
            if (x > y) {
                if (x > z)
                    return 0;
                else
                    return 2;
            } else {
                if (y > z)
                    return 1;
                else
                    return 2;
            }
        }

        uint32 minDim() const {
            if (x < y) {
                if (x < z)
                    return 0;
                else
                    return 2;
            } else {
                if (y < z)
                    return 1;
                else
                    return 2;
            }
        }

        T operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        T& operator[](uint32 idx) {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        bool operator==(Vector3<T> vec) const {
            return x == vec.x && y == vec.y && z == vec.z;
        }

        Float cube() const {
            return x * y * z;
        }
    };

    template<typename T>
    inline Vector3<T> operator*(T scalar, const Vector3<T>& vec) {
        return vec * scalar;
    }

    template<typename T>
    inline Vector3<T> operator/(T scalar, const Vector3<T>& vec) {
        // Error, not allowed
        return Vector3<T>();
    }

    template<typename T>
    inline Vector3<T> abs(const Vector3<T>& vec) {
        return Vector3<T>(std::abs(vec.x), std::abs(vec.y), std::abs(vec.z));
    }

    template<typename T>
    inline T dot(const Vector3<T>& vec1, const Vector3<T> vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    template<typename T>
    inline T absDot(const Vector3<T>& vec1, const Vector3<T> vec2) {
        return std::abs(dot(vec1, vec2));
    }

    template<typename T>
    inline Vector3<T> cross(const Vector3<T>& vec1, const Vector3<T> vec2) {
        double vec1x = vec1.x, vec1y = vec1.y, vec1z = vec1.z;
        double vec2x = vec2.x, vec2y = vec2.y, vec2z = vec2.z;

        return Vector3<T>((vec1y * vec2z) - (vec1z * vec2y),
                          (vec1z * vec2x) - (vec1x * vec2z),
                          (vec1x * vec2y) - (vec1y * vec2x));
    }

    template<typename T>
    inline Vector3<T> normalize(const Vector3<T>& vec) {
        return vec / vec.length();
    }

    template<typename T>
    inline Vector3<T> sign(const Vector3<T>& vec) {
        return Vector3<T>(sign(vec.x), sign(vec.y), sign(vec.z));
    }

    template<typename T>
    inline Vector3<T> min(const Vector3<T>& vec1, const Vector3<T>& vec2) {
        return Vector3<T>(std::min(vec1.x, vec2.x),
                          std::min(vec1.y, vec2.y),
                          std::min(vec1.z, vec2.z));
    }

    template<typename T>
    inline Vector3<T> max(const Vector3<T>& vec1, const Vector3<T>& vec2) {
        return Vector3<T>(std::max(vec1.x, vec2.x),
                            std::max(vec1.y, vec2.y),
                            std::max(vec1.z, vec2.z));
    }

    template<typename T>
    inline T min(const Vector3<T>& vec) {
        return std::min(vec.x, std::min(vec.y, vec.z));
    }

    template<typename T>
    inline T max(const Vector3<T>& vec) {
        return std::max(vec.x, std::max(vec.y, vec.z));
    }

    template<typename T>
    inline uint32 maxDim(const Vector3<T>& vec) {
        if (vec.x > vec.y) {
            if (vec.x > vec.z)
                return 0;
            else
                return 2;
        } else {
            if (vec.y > vec.z)
                return 1;
            else
                return 2;
        }
    }

    template<typename T>
    inline uint32 minDim(const Vector3<T>& vec) {
        if (vec.x < vec.y) {
            if (vec.x < vec.z)
                return 0;
            else
                return 2;
        } else {
            if (vec.y < vec.z)
                return 1;
            else
                return 2;
        }
    }

    template<typename T>
    inline void basisFromVector(const Vector3<T>& vec1, Vector3<T>* vec2, Vector3<T>* vec3) {
        /*if (std::abs(vec1.x) > std::abs(vec1.y)) {
            Float len = std::sqrt(vec1.x * vec1.x + vec1.z * vec1.z);
            *vec2 = Vector3<T>(-vec1.z, 0, vec1.x) / len;
        } else {
            Float len = std::sqrt(vec1.y * vec1.y + vec1.z * vec1.z);
            *vec2 = Vector3<T>(0, vec1.z, -vec1.y) / len;
        }
        *vec3 = cross(vec1, *vec2);
        */

        // [Duff et. al, 2017] "Building an Orthonormal Basis, Revisited"
        float sign = std::copysign(1.0f, vec1.z);
        const float a = -1.0 / (sign + vec1.z);
        const float b = vec1.x * vec1.y * a;

        *vec2 = Vec3(1.0 + sign * vec1.x * vec1.x * a, sign * b, -sign * vec1.x);
        *vec3 = Vec3(b, sign + vec1.y * vec1.y * a, -vec1.y);
    }

    template<typename T>
    inline Vector3<T> clamp(Vector3<T> val, T low, T high) {
        Vector3<T> ret;
        for (int32 i = 0; i < 3; i++)
            ret[i] = clamp<T, T, T>(val[i], low, high);
        return ret;
    }

    template<typename T>
    inline Vector3<T> clamp(Vector3<T> val, Vector3<T> low, Vector3<T> high) {
        Vector3<T> ret;
        for (int32 i = 0; i < 3; i++)
            ret[i] = clamp<T, T, T>(val[i], low[i], high[i]);
        return ret;
    }


    template<typename T>
    class Vector2 {
    public:
        T x, y;

        Vector2() : x(0), y(0) {}
        Vector2(T scalar) : x(scalar), y(scalar) {}
        Vector2(T x, T y) : x(x), y(y) {}

        template<typename U>
        Vector2(const Vector2<U>& vec) {
            x = T(vec.x);
            y = T(vec.y);
        }

        Vector2<T> operator+(const Vector2<T>& vec) const {
            return Vector2(x + vec.x, y + vec.y);
        }

        Vector2<T>& operator+=(const Vector2<T>& vec) {
            x += vec.x;
            y += vec.y;
            return *this;
        }

        Vector2<T> operator-(const Vector2<T>& vec) const {
            return Vector2(x - vec.x, y - vec.y);
        }

        Vector2<T>& operator-=(const Vector2<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            return *this;
        }

        Vector2<T> operator*(T scalar) const {
            return Vector2(x * scalar, y * scalar);
        }

        Vector2<T>& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Vector2<T> operator/(T scalar) const {
            T recip = 1 / scalar;
            return Vector2(x * recip, y * recip);
        }

        Vector2<T>& operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            return *this;
        }

        Vector2<T> operator-() const {
            return Vector2<T>(-x, -y);
        }

        Float lengthSqr() const {
            return x * x + y * y;
        }

        Float length() const {
            return std::sqrt(lengthSqr());
        }

        void normalize() {
            *this /= length();
        }

        T min() const {
            return std::min(x, y);
        }

        T max() const {
            return std::max(x, y);
        }

        uint32 maxDim() const {
            if (x > y)
                return 0;
            else
                return 1;
        }

        uint32 minDim() const {
            if (x < y)
                return 0;
            else
                return 1;
        }

        T operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            return y;
        }

        T& operator[](uint32 idx) {
            if (idx == 0)
                return x;

            return y;
        }

        bool operator==(Vector2<T> vec) const {
            return x == vec.x && y == vec.y;
        }
    };


    template<typename T>
    inline Vector2<T> operator*(T scalar, const Vector2<T>& vec) {
        return vec * scalar;
    }

    template<typename T>
    inline Vector2<T> operator/(T scalar, const Vector2<T>& vec) {
        // Error, not allowed
        return Vector2<T>();
    }

    template<typename T>
    inline Vector2<T> abs(const Vector2<T>& vec) {
        return Vector2<T>(std::abs(vec.x), std::abs(vec.y));
    }

    template<typename T>
    inline T dot(const Vector2<T>& vec1, const Vector2<T> vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y;
    }

    template<typename T>
    inline T absDot(const Vector2<T>& vec1, const Vector2<T> vec2) {
        return std::abs(dot(vec1, vec2));
    }

    template<typename T>
    inline Vector2<T> normalize(const Vector2<T>& vec) {
        return vec / vec.length();
    }

    template<typename T>
    inline Vector2<T> min(const Vector2<T>& vec1, const Vector2<T>& vec2) {
        return Vector2<T>(std::min(vec1.x, vec2.x),
                            std::min(vec1.y, vec2.y));
    }

    template<typename T>
    inline Vector2<T> max(const Vector2<T>& vec1, const Vector2<T>& vec2) {
        return Vector2<T>(std::max(vec1.x, vec2.x),
                            std::max(vec1.y, vec2.y));
    }

    template<typename T>
    inline T min(const Vector2<T>& vec) {
        return std::min(vec.x, vec.y);
    }

    template<typename T>
    inline T max(const Vector2<T>& vec) {
        return std::max(vec.x, vec.y);
    }

    template<typename T>
    inline uint32 maxDim(const Vector2<T>& vec) {
        if (vec.x > vec.y)
            return 0;
        else
            return 1;
    }

    template<typename T>
    inline uint32 minDim(const Vector2<T>& vec) {
        if (vec.x < vec.y)
            return 0;
        else
            return 1;
    }





    template<typename T>
    class Point3T {
    public:
        T x, y, z;

        Point3T() : x(0), y(0), z(0) {}
        Point3T(T scalar) : x(scalar), y(scalar), z(scalar) {}
        Point3T(T x, T y, T z) : x(x), y(y), z(z) {}

        template<typename U>
        Point3T(const Point3T<U>& pt) {
            x = T(pt.x);
            y = T(pt.y);
            z = T(pt.z);
        }

        Point3T<T> operator+(const Vector3<T>& vec) const {
            return Point3T<T>(x + vec.x, y + vec.y, z + vec.z);
        }

        Point3T<T>& operator+=(const Vector3<T>& vec) {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        Point3T<T> operator-(const Vector3<T>& vec) const {
            return Point3T<T>(x - vec.x, y - vec.y, z - vec.z);
        }

        Vector3<T> operator-(const Point3T<T>& pt) const {
            return Vector3<T>(x - pt.x, y - pt.y, z - pt.z);
        }

        Point3T<T>& operator-=(const Vector3<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        Point3T<T> operator*(const Point3T<T>& pt) const {
            return Point3T(x * pt.x, y * pt.y, z * pt.z);
        }

        Point3T<T> operator*(T scalar) const {
            return Point3T(x * scalar, y * scalar, z * scalar);
        }

        Point3T<T>& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Point3T<T> operator/(T scalar) const {
            T recip = 1 / scalar;
            return Point3T(x * recip, y * recip, z * recip);
        }

        Point3T<T>& operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            z *= recip;
            return *this;
        }

        Point3T<T> operator-() const {
            return Point3T<T>(-x, -y, -z);
        }

        Vector3<T> posVec() const {
            return Vector3<T>(x, y, z);
        }

        T min() const {
            return std::min(x, std::min(y, z));
        }

        T max() const {
            return std::max(x, std::max(y, z));
        }

        uint32 maxDim() const {
            if (x > y) {
                if (x > z)
                    return 0;
                else
                    return 2;
            } else {
                if (y > z)
                    return 1;
                else
                    return 2;
            }
        }

        uint32 minDim() const {
            if (x < y) {
                if (x < z)
                    return 0;
                else
                    return 2;
            } else {
                if (y < z)
                    return 1;
                else
                    return 2;
            }
        }

        T operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        T& operator[](uint32 idx) {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        bool operator==(Point3T<T> pt) const {
            return x == pt.x && y == pt.y && z == pt.z;
        }

        bool infinity() const {
            return x == std::numeric_limits<T>::infinity() ||
                   y == std::numeric_limits<T>::infinity() || 
                   z == std::numeric_limits<T>::infinity();
        }
    };

    template<typename T>
    inline Point3T<T> operator*(T scalar, const Point3T<T>& pt) {
        return pt * scalar;
    }

    template<typename T>
    inline Point3T<T> operator/(T scalar, const Point3T<T>& pt) {
        // Error, not allowed
        return Point3T();
    }

    template<typename T>
    inline Point3T<T> abs(const Point3T<T>& pt) {
        return Point3T<T>(std::abs(pt.x), std::abs(pt.y), std::abs(pt.z));
    }

    template<typename T>
    inline Point3T<T> min(const Point3T<T>& pt1, const Point3T<T>& pt2) {
        return Point3T<T>(std::min(pt1.x, pt2.x),
                          std::min(pt1.y, pt2.y),
                          std::min(pt1.z, pt2.z));
    }

    template<typename T>
    inline Point3T<T> max(const Point3T<T>& pt1, const Point3T<T>& pt2) {
        return Point3T<T>(std::max(pt1.x, pt2.x),
                          std::max(pt1.y, pt2.y),
                          std::max(pt1.z, pt2.z));
    }

    template<typename T>
    inline T min(const Point3T<T>& pt) {
        return std::min(pt.x, std::min(pt.y, pt.z));
    }

    template<typename T>
    inline T max(const Point3T<T>& pt) {
        return std::max(pt.x, std::max(pt.y, pt.z));
    }

    template<typename T>
    inline uint32 maxDim(const Point3T<T>& pt) {
        if (pt.x > pt.y) {
            if (pt.x > pt.z)
                return 0;
            else
                return 2;
        } else {
            if (pt.y > pt.z)
                return 1;
            else
                return 2;
        }
    }

    template<typename T>
    inline uint32 minDim(const Point3T<T>& pt) {
        if (pt.x < pt.y) {
            if (pt.x < pt.z)
                return 0;
            else
                return 2;
        } else {
            if (pt.y < pt.z)
                return 1;
            else
                return 2;
        }
    }

    template<typename T>
    inline Float dist(const Point3T<T>& pt1, const Point3T<T>& pt2) {
        return (pt1 - pt2).length();
    }

    template<typename T>
    inline Float distSqr(const Point3T<T>& pt1, const Point3T<T>& pt2) {
        return (pt1 - pt2).lengthSqr();
    }

    template<typename T>
    Point3T<T> lerp(Float t, const Point3T<T>& p1, const Point3T<T>& p2) {
        return (1 - t) * p1 + t * p2;
    }



    template<typename T>
    class Point2T {
    public:
        T x, y;

        Point2T() : x(0), y(0) {}
        Point2T(T scalar) : x(scalar), y(scalar) {}
        Point2T(T x, T y) : x(x), y(y) {}

        Point2T<T> operator+(const Vector2<T>& vec) const {
            return Point2T<T>(x + vec.x, y + vec.y);
        }

        Point2T<T>& operator+=(const Vector2<T>& vec) {
            x += vec.x;
            y += vec.y;
            return *this;
        }

        Point2T<T> operator-(const Vector2<T>& vec) const {
            return Point2T<T>(x - vec.x, y - vec.y);
        }

        Vector2<T> operator-(const Point2T<T>& pt) const {
            return Vector2<T>(x - pt.x, y - pt.y);
        }

        Point2T<T>& operator-=(const Vector2<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            return *this;
        }

        Point2T<T> operator*(T scalar) const {
            return Point2T(x * scalar, y * scalar);
        }

        Point2T<T>& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        Point2T<T> operator/(T scalar) const {
            T recip = 1 / scalar;
            return Vector2(x * recip, y * recip);
        }

        Point2T<T>& operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            return *this;
        }

        Point2T<T> operator-() const {
            return Point2T<T>(-x, -y);
        }

        Vector2<T> posVec() const {
            return Vector2<T>(x, y);
        }

        T min() const {
            return std::min(x, y);
        }

        T max() const {
            return std::max(x, y);
        }

        uint32 maxDim() const {
            if (x > y)
                return 0;
            else 
                return 1;
        }

        uint32 minDim() const {
            if (x < y) 
                return 0;
            else
                return 1;
        }

        T operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            return y;
        }

        T& operator[](uint32 idx) {
            if (idx == 0)
                return x;

            return y;
        }
    };

    template<typename T>
    inline Point2T<T> operator*(T scalar, const Point2T<T>& pt) {
        return pt * scalar;
    }

    template<typename T>
    inline Point2T<T> operator/(T scalar, const Point2T<T>& pt) {
        // Error, not allowed
        return Point2T();
    }

    template<typename T>
    inline Point2T<T> abs(const Point2T<T>& pt) {
        return Vector2<T>(std::abs(pt.x), std::abs(pt.y));
    }

    template<typename T>
    inline Point2T<T> min(const Point2T<T>& pt1, const Point2T<T>& pt2) {
        return Point2T<T>(std::min(pt1.x, pt2.x),
                          std::min(pt1.y, pt2.y));
    }

    template<typename T>
    inline Point2T<T> max(const Point2T<T>& pt1, const Point2T<T>& pt2) {
        return Point2T<T>(std::max(pt1.x, pt2.x),
                          std::max(pt1.y, pt2.y));
    }

    template<typename T>
    inline T min(const Point2T<T>& pt) {
        return std::min(pt.x, pt.y);
    }

    template<typename T>
    inline T max(const Point2T<T>& pt) {
        return std::max(pt.x, pt.y);
    }

    template<typename T>
    inline uint32 maxDim(const Point2T<T>& pt) {
        if (pt.x > pt.y)
            return 0;
        else
            return 1;
    }

    template<typename T>
    inline uint32 minDim(const Point2T<T>& pt) {
        if (pt.x < pt.y) 
            return 0;
        else 
            return 1;
    }

    template<typename T>
    inline Float dist(const Point2T<T>& pt1, const Point2T<T>& pt2) {
        return (pt1 - pt2).length();
    }

    template<typename T>
    inline Float distSqr(const Point2T<T>& pt1, const Point2T<T>& pt2) {
        return (pt1 - pt2).lengthSqr();
    }

    template<typename T>
    Point2T<T> lerp(Float t, const Point2T<T>& p1, const Point2T<T>& p2) {
        return (1 - t) * p1 + t * p2;
    }




    template<typename T>
    class Normal3 {
    public:
        T x, y, z;

        Normal3() : x(0), y(0), z(0) {}
        Normal3(T scalar) : x(scalar), y(scalar), z(scalar) {}
        Normal3(T x, T y, T z) : x(x), y(y), z(z) {}
        explicit Normal3(const Vector3<T>& vec) : x(vec.x), y(vec.y), z(vec.z) {}

        Normal3<T> operator+(const Normal3<T>& vec) const {
            return Normal3<T>(x + vec.x, y + vec.y, z + vec.z);
        }

        Normal3<T> operator+(const Vector3<T>& vec) const {
            return Vector3<T>(x + vec.x, y + vec.y, z + vec.z);
        }

        Normal3<T>& operator+=(const Normal3<T>& vec) {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        Normal3<T>& operator+=(const Vector3<T>& vec) {
            x += vec.x;
            y += vec.y;
            z += vec.z;
            return *this;
        }

        Normal3<T> operator-(const Normal3<T>& vec) const {
            return Normal3(x - vec.x, y - vec.y, z - vec.z);
        }

        Normal3<T> operator-(const Vector3<T>& vec) const {
            return Normal3(x - vec.x, y - vec.y, z - vec.z);
        }

        Normal3<T>& operator-=(const Normal3<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        Normal3<T>& operator-=(const Vector3<T>& vec) {
            x -= vec.x;
            y -= vec.y;
            z -= vec.z;
            return *this;
        }

        Normal3<T> operator*(T scalar) const {
            return Normal3(x * scalar, y * scalar, z * scalar);
        }

        Normal3<T>& operator*=(T scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        Normal3<T> operator/(T scalar) const {
            T recip = 1 / scalar;
            return Normal3<T>(x * recip, y * recip, z * recip);
        }

        Normal3<T>& operator/=(T scalar) {
            T recip = 1 / scalar;
            x *= recip;
            y *= recip;
            z *= recip;
            return *this;
        }

        Normal3<T> operator-() const {
            return Normal3<T>(-x, -y, -z);
        }

        Float lengthSqr() const {
            return x * x + y * y + z * z;
        }

        Float length() const {
            return std::sqrt(lengthSqr());
        }

        void normalize() {
            *this /= length();
        }

        T min() const {
            return std::min(x, std::min(y, z));
        }

        T max() const {
            return std::max(x, std::max(y, z));
        }

        uint32 maxDim() const {
            if (x > y) {
                if (x > z)
                    return 0;
                else
                    return 2;
            } else {
                if (y > z)
                    return 1;
                else
                    return 2;
            }
        }

        uint32 minDim() const {
            if (x < y) {
                if (x < z)
                    return 0;
                else
                    return 2;
            } else {
                if (y < z)
                    return 1;
                else
                    return 2;
            }
        }

        T operator[](uint32 idx) const {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

        T& operator[](uint32 idx) {
            if (idx == 0)
                return x;

            if (idx == 1)
                return y;

            return z;
        }

    };

    template<typename T>
    inline Vector3<T>::Vector3(const Normal3<T>& normal)
        : x(normal.x), y(normal.y), z(normal.z) {}

    template<typename T>
    inline Normal3<T> operator*(T scalar, const Normal3<T>& vec) {
        return vec * scalar;
    }

    template<typename T>
    inline Normal3<T> operator/(T scalar, const Normal3<T>& vec) {
        // Error, not allowed
        return Normal3<T>();
    }

    template<typename T>
    inline Normal3<T> abs(const Normal3<T>& vec) {
        return Normal3<T>(std::abs(vec.x), std::abs(vec.y), std::abs(vec.z));
    }

    template<typename T>
    inline T dot(const Normal3<T>& vec1, const Normal3<T> vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    template<typename T>
    inline T dot(const Normal3<T>& vec1, const Vector3<T> vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    template<typename T>
    inline T dot(const Vector3<T>& vec1, const Normal3<T> vec2) {
        return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
    }

    template<typename T>
    inline T absDot(const Normal3<T>& vec1, const Normal3<T> vec2) {
        return std::abs(dot(vec1, vec2));
    }

    template<typename T>
    inline Normal3<T> normalize(const Normal3<T>& vec) {
        return vec / vec.length();
    }

    template<typename T>
    inline Normal3<T> min(const Normal3<T>& vec1, const Normal3<T>& vec2) {
        return Normal3<T>(std::min(vec1.x, vec2.x),
                            std::min(vec1.y, vec2.y),
                            std::min(vec1.z, vec2.z));
    }

    template<typename T>
    inline Normal3<T> max(const Normal3<T>& vec1, const Normal3<T>& vec2) {
        return Normal3<T>(std::max(vec1.x, vec2.x),
                            std::max(vec1.y, vec2.y),
                            std::max(vec1.z, vec2.z));
    }

    template<typename T>
    inline T min(const Normal3<T>& vec) {
        return std::min(vec.x, std::min(vec.y, vec.z));
    }

    template<typename T>
    inline T max(const Normal3<T>& vec) {
        return std::max(vec.x, std::max(vec.y, vec.z));
    }

    template<typename T>
    inline uint32 maxDim(const Normal3<T>& vec) {
        if (vec.x > vec.y) {
            if (vec.x > vec.z)
                return 0;
            else
                return 2;
        } else {
            if (vec.y > vec.z)
                return 1;
            else
                return 2;
        }
    }

    template<typename T>
    inline uint32 minDim(const Normal3<T>& vec) {
        if (vec.x < vec.y) {
            if (vec.x < vec.z)
                return 0;
            else
                return 2;
        } else {
            if (vec.y < vec.z)
                return 1;
            else
                return 2;
        }
    }

}

#if PHOTON_USE_GLM

    typedef glm::vec2  Vec2;
    typedef glm::vec3  Vec3;
    typedef glm::vec4  Vec4;

    typedef glm::vec3  Color3;
    typedef glm::vec4  Color4;

    typedef glm::uvec2 Vec2u;

#else

    typedef Math::Vector3<Float> Vec3;
    typedef Math::Vector3<int32> Vec3i;
    typedef Math::Vector3<uint32> Vec3ui;

    typedef Math::Vector2<Float> Vec2;
    typedef Math::Vector2<int32> Vec2i;
    typedef Math::Vector2<uint32> Vec2ui;

    typedef Math::Point2T<Float> Point2;
    typedef Math::Point2T<int32> Point2i;
    typedef Math::Point2T<uint32> Point2ui;
    
    typedef Math::Point3T<Float> Point3;
    typedef Math::Point3T<int32> Point3i;
    typedef Math::Point3T<uint32> Point3ui;

    typedef Math::Normal3<Float> Normal;

    typedef Math::Vector3<Float> Color3;

#endif
}