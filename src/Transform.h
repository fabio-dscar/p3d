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

        const Mat4& matrix() const;
        const Mat4& invMatrix() const;

        Vec3    operator()(const Vec3& vec)    const;
        Point3  operator()(const Point3& pt)   const;
        Normal  operator()(const Normal& norm) const;     
        Ray     operator()(const Ray& ray)     const;
        Bounds3 operator()(const Bounds3& box) const;

        Transform operator*(const Transform& t);

        bool flipsOrientation() const;
    private:
        Mat4 _mat;
        Mat4 _invMat;
    };

    // Transform utility functions
    Transform inverse  (const Transform& trans);
    Transform translate(const Vec3& trans);
    Transform scale    (Float sX, Float sY, Float sZ);
    Transform rotateX  (Float degrees);
    Transform rotateY  (Float degrees);
    Transform rotateZ  (Float degrees);
    Transform ortho    (Float zNear, Float zFar);
    Transform lookAt   (const Point3& pos, const Point3& at, const Vec3& up);

}