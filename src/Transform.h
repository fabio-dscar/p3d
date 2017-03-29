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
        Transform(const Mat4& matrix)
            : _mat(matrix), _invMat(inverse(matrix)) { }
        Transform(const Mat4& matrix, const Mat4& invMatrix)
            : _mat(matrix), _invMat(invMatrix) { }

        template<typename T>
        Point3T<T> operator()(const Point3T<T>& pt) const {
            T x = _mat.m[0][0] * pt.x + _mat.m[0][1] * pt.y + _mat.m[0][2] * pt.z + _mat.m[0][3];
            T y = _mat.m[1][0] * pt.x + _mat.m[1][1] * pt.y + _mat.m[1][2] * pt.z + _mat.m[1][3];
            T z = _mat.m[2][0] * pt.x + _mat.m[2][1] * pt.y + _mat.m[2][2] * pt.z + _mat.m[2][3];
            T w = _mat.m[3][0] * pt.x + _mat.m[3][1] * pt.y + _mat.m[3][2] * pt.z + _mat.m[3][3];

            if (w == 1)
                return Point3T<T>(x, y, z);
            else
                return Point3T<T>(x, y, z) / w;
        }

        template<typename T>
        Vector3<T> operator()(const Vector3<T>& vec) const {
            T x = _mat.m[0][0] * vec.x + _mat.m[0][1] * vec.y + _mat.m[0][2] * vec.z;
            T y = _mat.m[1][0] * vec.x + _mat.m[1][1] * vec.y + _mat.m[1][2] * vec.z;
            T z = _mat.m[2][0] * vec.x + _mat.m[2][1] * vec.y + _mat.m[2][2] * vec.z;
            
            return Vector3<T>(x, y, z);
        }

        template<typename T>
        Normal3<T> operator()(const Normal3<T>& norm) const {
            T x = _invMat.m[0][0] * norm.x + _invMat.m[1][0] * norm.y + _invMat.m[2][0] * norm.z;
            T y = _invMat.m[0][1] * norm.x + _invMat.m[1][1] * norm.y + _invMat.m[2][1] * norm.z;
            T z = _invMat.m[0][2] * norm.x + _invMat.m[1][2] * norm.y + _invMat.m[2][2] * norm.z;

            return Normal3<T>(x, y, z);
        }

        Ray operator()(const Ray& ray) const {
            Point3 orig = (*this)(ray.origin());
            Vec3 dir = (*this)(ray.dir());

            return Ray(orig, dir, ray.minT(), ray.maxT());
        }

        Bounds3 Transform::operator()(const Bounds3 &box) const {
            const Transform &Tr = *this;

            Bounds3 ret(Point3(0), Tr(Point3(box[0].x, box[0].y, box[0].z)));
            ret = expand(ret, Tr(Point3(box[1].x, box[0].y, box[0].z)));
            ret = expand(ret, Tr(Point3(box[0].x, box[1].y, box[0].z)));
            ret = expand(ret, Tr(Point3(box[0].x, box[0].y, box[1].z)));
            ret = expand(ret, Tr(Point3(box[0].x, box[1].y, box[1].z)));
            ret = expand(ret, Tr(Point3(box[1].x, box[1].y, box[0].z)));
            ret = expand(ret, Tr(Point3(box[1].x, box[0].y, box[1].z)));
            ret = expand(ret, Tr(Point3(box[1].x, box[1].y, box[1].z)));

            return ret;
        }

        Transform operator*(const Transform& t) {
            return Transform(mul(_mat, t._mat),
                             mul(t._invMat, _invMat));
        }

    private:
        Mat4 _mat;
        Mat4 _invMat;
    };

    Transform translate(const Vec3 trans) {
        Mat4 mat(1, 0, 0, trans.x,
                 0, 1, 0, trans.y,
                 0, 0, 1, trans.z,
                 0, 0, 0,       1);

        Mat4 invMat(1, 0, 0, -trans.x,
                    0, 1, 0, -trans.y,
                    0, 0, 1, -trans.z,
                    0, 0, 0,        1);

        return Transform(mat, invMat);
    }

    Transform scale(Float sX, Float sY, Float sZ) {
        Mat4 mat(sX,  0,  0, 0,
                  0, sY,  0, 0,
                  0,  0, sZ, 0,
                  0,  0,  0, 1);

        Mat4 invMat(1 / sX, 0, 0, 0,
                    0, 1 / sY, 0, 0,
                    0, 0, 1 / sZ, 0,
                    0, 0, 0, 1);

        return Transform(mat, invMat);
    }

    Transform rotateX(Float degrees) {
        Float rads = radians(degrees);
        Float sin = std::sin(rads);
        Float cos = std::cos(rads);

        Mat4 mat(1,   0,    0, 0,
                 0, cos, -sin, 0,
                 0, sin,  cos, 0,
                 0,   0,    0, 1);

        // The inverse of rotation matrices is their own transpose 
        return Transform(mat, transpose(mat));
    }

    Transform rotateY(Float degrees) {
        Float rads = radians(degrees);
        Float sin = std::sin(rads);
        Float cos = std::cos(rads);

        Mat4 mat(cos, 0, sin, 0,
                   0, 1,   0, 0,
                -sin, 0, cos, 0,
                   0, 0,   0, 1);

        // The inverse of rotation matrices is their own transpose 
        return Transform(mat, transpose(mat));
    }

    Transform rotateZ(Float degrees) {
        Float rads = radians(degrees);
        Float sin = std::sin(rads);
        Float cos = std::cos(rads);

        Mat4 mat(cos, -sin, 0, 0,
                 sin,  cos, 0, 0,
                   0,    0, 1, 0,
                   0,    0, 0, 1);

        // The inverse of rotation matrices is their own transpose 
        return Transform(mat, transpose(mat));
    }

    Transform ortho(Float zNear, Float zFar) {
        return scale(1, 1, 1 / (zFar - zNear)) * translate(Vec3(0, 0, -zNear));
    }



}