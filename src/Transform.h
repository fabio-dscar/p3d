#pragma once

#include <Vector.h>
#include <Matrix.h>
#include <Ray.h>
#include <Bounds.h>

using namespace Photon::Math;

namespace Photon {

    class Transform {
    public:
        Transform() : _mat(), _invMat() { }
        Transform(const Mat4& matrix);
        Transform(const Mat4& matrix, const Mat4& invMatrix)
            : _mat(matrix), _invMat(invMatrix) { }

        template<typename T>
        Point3T<T> operator()(const Point3T<T>& pt) const;

        template<typename T>
        Vector3<T> operator()(const Vector3<T>& vec) const;

        template<typename T>
        Normal3<T> operator()(const Normal3<T>& norm) const;

        Ray     operator()(const Ray& ray) const;
        Bounds3 operator()(const Bounds3 &box) const;

        Transform operator*(const Transform& t);

    private:
        Mat4 _mat;
        Mat4 _invMat;
    };

    // Transform utility functions
    Transform translate(const Vec3 trans);
    Transform scale    (Float sX, Float sY, Float sZ);
    Transform rotateX  (Float degrees);
    Transform rotateY  (Float degrees);
    Transform rotateZ  (Float degrees);
    Transform ortho    (Float zNear, Float zFar);

}