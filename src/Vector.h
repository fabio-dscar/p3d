#pragma once

#include <PhotonMath.h>

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
        Vector3(const Vector3<U>& vec);

        Vector3<T>  operator+ (const Vector3<T>& vec) const;
        Vector3<T>& operator+=(const Vector3<T>& vec);

        Vector3<T>  operator- (const Vector3<T>& vec) const;
        Vector3<T>& operator-=(const Vector3<T>& vec);

        Vector3<T>  operator* (const Vector3<T>& vec) const;
        Vector3<T>& operator*=(const Vector3<T>& vec);
        Vector3<T>  operator* (T scalar) const;
        Vector3<T>& operator*=(T scalar);

        Vector3<T>  operator/ (T scalar) const;
        Vector3<T>& operator/=(T scalar);

        Vector3<T>  operator-() const;

        T  operator[](uint32 idx) const;
        T& operator[](uint32 idx);

        bool operator==(Vector3<T> vec) const;

        Vector3<T> recip() const;

        Float lengthSqr() const;
        Float length() const;

        Float cube() const;

        void normalize();

        T min() const;
        T max() const;
        uint32 maxDim() const;
        uint32 minDim() const;

        bool isZero() const;
    };

    template<typename T>
    inline Vector3<T> operator*(T scalar, const Vector3<T>& vec);

    template<typename T>
    inline Vector3<T> operator/(T scalar, const Vector3<T>& vec);

    template<typename T>
    inline Vector3<T> abs(const Vector3<T>& vec);

    template<typename T>
    inline T dot(const Vector3<T>& vec1, const Vector3<T>& vec2);

    template<typename T>
    inline T absDot(const Vector3<T>& vec1, const Vector3<T>& vec2);

    template<typename T>
    inline Vector3<T> cross(const Vector3<T>& vec1, const Vector3<T>& vec2);

    template<typename T>
    inline Vector3<T> normalize(const Vector3<T>& vec);

    template<typename T>
    inline Vector3<T> sign(const Vector3<T>& vec);

    template<typename T>
    inline Vector3<T> min(const Vector3<T>& vec1, const Vector3<T>& vec2);

    template<typename T>
    inline Vector3<T> max(const Vector3<T>& vec1, const Vector3<T>& vec2);

    template<typename T>
    inline T min(const Vector3<T>& vec);

    template<typename T>
    inline T max(const Vector3<T>& vec);

    template<typename T>
    inline uint32 maxDim(const Vector3<T>& vec);

    template<typename T>
    inline uint32 minDim(const Vector3<T>& vec);

    template<typename T>
    inline void basisFromVector(const Vector3<T>& vec1, Vector3<T>* vec2, Vector3<T>* vec3);

    template<typename T>
    inline Vector3<T> clamp(Vector3<T> val, T low, T high);

    template<typename T>
    inline Vector3<T> clamp(Vector3<T> val, Vector3<T> low, Vector3<T> high);

    template<typename T>
    inline Vector3<T> pow(Vector3<T> val, Float exp);

    template<typename T>
    inline Vector3<T> floor(Vector3<T> vec);

    template<typename T>
    inline Vector3<T> ceil(Vector3<T> vec);

    template<typename T>
    class Vector2 {
    public:
        T x, y;

        Vector2() : x(0), y(0) {}
        Vector2(T scalar) : x(scalar), y(scalar) {}
        Vector2(T x, T y) : x(x), y(y) {}
        Vector2(const Vector3<T>& vec) : x(vec.x), y(vec.y) {}

        template<typename U>
        Vector2(const Vector2<U>& vec);

        Vector2<T>  operator+ (const Vector2<T>& vec) const;
        Vector2<T>& operator+=(const Vector2<T>& vec);

        Vector2<T>  operator- (const Vector2<T>& vec) const;
        Vector2<T>& operator-=(const Vector2<T>& vec);

        Vector2<T>  operator* (const Vector2<T>& vec) const;
        Vector2<T>& operator*=(const Vector2<T>& vec);
        Vector2<T>  operator* (T scalar) const;
        Vector2<T>& operator*=(T scalar);

        Vector2<T>  operator/ (T scalar) const;
        Vector2<T>& operator/=(T scalar);

        Vector2<T>  operator-() const;

        T  operator[](uint32 idx) const;
        T& operator[](uint32 idx);

        bool operator==(Vector2<T> vec) const;

        Float lengthSqr() const;
        Float length() const;

        void normalize();

        T min() const;
        T max() const;
        uint32 maxDim() const;
        uint32 minDim() const;
    };

    template<typename T>
    inline Vector2<T> operator*(T scalar, const Vector2<T>& vec);

    template<typename T>
    inline Vector2<T> operator/(T scalar, const Vector2<T>& vec);

    template<typename T>
    inline Vector2<T> abs(const Vector2<T>& vec);

    template<typename T>
    inline T dot(const Vector2<T>& vec1, const Vector2<T>& vec2);

    template<typename T>
    inline T absDot(const Vector2<T>& vec1, const Vector2<T>& vec2);

    template<typename T>
    inline Vector2<T> normalize(const Vector2<T>& vec);

    template<typename T>
    inline Vector2<T> min(const Vector2<T>& vec1, const Vector2<T>& vec2);

    template<typename T>
    inline Vector2<T> max(const Vector2<T>& vec1, const Vector2<T>& vec2);

    template<typename T>
    inline T min(const Vector2<T>& vec);

    template<typename T>
    inline T max(const Vector2<T>& vec);

    template<typename T>
    inline uint32 maxDim(const Vector2<T>& vec);

    template<typename T>
    inline uint32 minDim(const Vector2<T>& vec);


    template<typename T>
    class Point3T {
    public:
        T x, y, z;

        Point3T() : x(0), y(0), z(0) {}
        Point3T(T scalar) : x(scalar), y(scalar), z(scalar) {}
        Point3T(T x, T y, T z) : x(x), y(y), z(z) {}

        template<typename U>
        Point3T(const Point3T<U>& pt);

        Point3T<T>  operator+ (const Point3T<T>& pt) const;
        Point3T<T>  operator+ (const Vector3<T>& vec) const;
        Point3T<T>& operator+=(const Vector3<T>& vec);

        Vector3<T>  operator- (const Point3T<T>& pt) const;
        Point3T<T>  operator- (const Vector3<T>& vec) const;
        Point3T<T>& operator-=(const Vector3<T>& vec);

        Point3T<T>  operator* (const Point3T<T>& pt) const;
        Point3T<T>  operator* (T scalar) const;
        Point3T<T>& operator*=(T scalar);

        Point3T<T>  operator/ (T scalar) const;
        Point3T<T>& operator/=(T scalar);

        Point3T<T> operator-() const;

        T operator[] (uint32 idx) const;
        T& operator[](uint32 idx);

        bool operator==(Point3T<T> pt) const;

        Vector3<T> posVec() const;

        T min() const;
        T max() const;
        uint32 maxDim() const;
        uint32 minDim() const;
     
        bool infinity() const;
    };

    template<typename T>
    inline Point3T<T> operator*(T scalar, const Point3T<T>& pt);

    template<typename T>
    inline Point3T<T> operator/(T scalar, const Point3T<T>& pt);

    template<typename T>
    inline Point3T<T> abs(const Point3T<T>& pt);

    template<typename T>
    inline Point3T<T> min(const Point3T<T>& pt1, const Point3T<T>& pt2);

    template<typename T>
    inline Point3T<T> max(const Point3T<T>& pt1, const Point3T<T>& pt2);

    template<typename T>
    inline T min(const Point3T<T>& pt);

    template<typename T>
    inline T max(const Point3T<T>& pt);

    template<typename T>
    inline uint32 maxDim(const Point3T<T>& pt);

    template<typename T>
    inline uint32 minDim(const Point3T<T>& pt);

    template<typename T>
    inline Float dist(const Point3T<T>& pt1, const Point3T<T>& pt2);

    template<typename T>
    inline Float distSqr(const Point3T<T>& pt1, const Point3T<T>& pt2);

    template<typename T>
    Point3T<T> lerp(Float t, const Point3T<T>& pt1, const Point3T<T>& pt2);


    template<typename T>
    class Point2T {
    public:
        T x, y;

        Point2T() : x(0), y(0) {}
        Point2T(T scalar) : x(scalar), y(scalar) {}
        Point2T(T x, T y) : x(x), y(y) {}
        Point2T(const Point3T<T>& pt) : x(pt.x), y(pt.y) {}

        Point2T<T>  operator+ (const Point2T<T>& pt) const;
        Point2T<T>  operator+ (const Vector2<T>& vec) const;
        Point2T<T>& operator+=(const Vector2<T>& vec);

        Point2T<T>  operator- (const Vector2<T>& vec) const;
        Vector2<T>  operator- (const Point2T<T>& pt) const;
        Point2T<T>& operator-=(const Vector2<T>& vec);

        Point2T<T>  operator* (T scalar) const;
        Point2T<T>& operator*=(T scalar);

        Point2T<T>  operator/ (T scalar) const;
        Point2T<T>& operator/=(T scalar);

        Point2T<T>  operator-() const;

        T operator[] (uint32 idx) const;
        T& operator[](uint32 idx);

        Vector2<T> posVec() const;

        T min() const;
        T max() const;
        uint32 maxDim() const;
        uint32 minDim() const;  
    };

    template<typename T>
    inline Point2T<T> operator*(T scalar, const Point2T<T>& pt);

    template<typename T>
    inline Point2T<T> operator/(T scalar, const Point2T<T>& pt);

    template<typename T>
    inline Point2T<T> abs(const Point2T<T>& pt);

    template<typename T>
    inline Point2T<T> min(const Point2T<T>& pt1, const Point2T<T>& pt2);

    template<typename T>
    inline Point2T<T> max(const Point2T<T>& pt1, const Point2T<T>& pt2);

    template<typename T>
    inline T min(const Point2T<T>& pt);

    template<typename T>
    inline T max(const Point2T<T>& pt);

    template<typename T>
    inline uint32 maxDim(const Point2T<T>& pt);

    template<typename T>
    inline uint32 minDim(const Point2T<T>& pt);

    template<typename T>
    inline Float dist(const Point2T<T>& pt1, const Point2T<T>& pt2);

    template<typename T>
    inline Float distSqr(const Point2T<T>& pt1, const Point2T<T>& pt2);

    template<typename T>
    Point2T<T> lerp(Float t, const Point2T<T>& p1, const Point2T<T>& p2);


    template<typename T>
    class Normal3 {
    public:
        T x, y, z;

        Normal3() : x(0), y(0), z(0) {}
        Normal3(T scalar) : x(scalar), y(scalar), z(scalar) {}
        Normal3(T x, T y, T z) : x(x), y(y), z(z) {}
        explicit Normal3(const Vector3<T>& vec) : x(vec.x), y(vec.y), z(vec.z) {}

        Normal3<T>  operator+ (const Normal3<T>& vec) const;
        Normal3<T>  operator+ (const Vector3<T>& vec) const;
        Normal3<T>& operator+=(const Normal3<T>& vec);
        Normal3<T>& operator+=(const Vector3<T>& vec);

        Normal3<T>  operator- (const Normal3<T>& vec) const;
        Normal3<T>  operator- (const Vector3<T>& vec) const;
        Normal3<T>& operator-=(const Normal3<T>& vec);
        Normal3<T>& operator-=(const Vector3<T>& vec);

        Normal3<T>  operator* (T scalar) const;
        Normal3<T>& operator*=(T scalar);

        Normal3<T>  operator/ (T scalar) const;
        Normal3<T>& operator/=(T scalar);

        Normal3<T> operator-() const;

        T operator[] (uint32 idx) const;
        T& operator[](uint32 idx);

        Float lengthSqr() const;
        Float length() const;

        void normalize();

        T min() const;
        T max() const;
        uint32 maxDim() const;
        uint32 minDim() const;
    };

    template<typename T>
    inline Normal3<T> operator*(T scalar, const Normal3<T>& vec);

    template<typename T>
    inline Normal3<T> operator/(T scalar, const Normal3<T>& vec);

    template<typename T>
    inline Normal3<T> abs(const Normal3<T>& vec);

    template<typename T>
    inline T dot(const Normal3<T>& vec1, const Normal3<T>& vec2);

    template<typename T>
    inline T dot(const Normal3<T>& vec1, const Vector3<T>& vec2);

    template<typename T>
    inline T dot(const Vector3<T>& vec1, const Normal3<T>& vec2);

    template<typename T>
    inline T absDot(const Normal3<T>& vec1, const Normal3<T>& vec2);

    template<typename T>
    inline T absDot(const Normal3<T>& vec1, const Vector3<T>& vec2);

    template<typename T>
    inline Normal3<T> normalize(const Normal3<T>& vec);

    template<typename T>
    inline Normal3<T> min(const Normal3<T>& vec1, const Normal3<T>& vec2);

    template<typename T>
    inline Normal3<T> max(const Normal3<T>& vec1, const Normal3<T>& vec2);

    template<typename T>
    inline T min(const Normal3<T>& vec);

    template<typename T>
    inline T max(const Normal3<T>& vec);

    template<typename T>
    inline uint32 maxDim(const Normal3<T>& vec);

    template<typename T>
    inline uint32 minDim(const Normal3<T>& vec);

}

    // Vector types definition
    typedef Math::Vector3<Float>  Vec3;
    typedef Math::Vector3<int32>  Vec3i;
    typedef Math::Vector3<uint32> Vec3ui;

    typedef Math::Vector2<Float>  Vec2;
    typedef Math::Vector2<int32>  Vec2i;
    typedef Math::Vector2<uint32> Vec2ui;

    typedef Math::Point2T<Float>  Point2;
    typedef Math::Point2T<int32>  Point2i;
    typedef Math::Point2T<uint32> Point2ui;
    
    typedef Math::Point3T<Float>  Point3;
    typedef Math::Point3T<int32>  Point3i;
    typedef Math::Point3T<uint32> Point3ui;

    typedef Math::Normal3<Float>  Normal;

    typedef Math::Vector3<Float>  Color3;

}

#include <Vector.inl>