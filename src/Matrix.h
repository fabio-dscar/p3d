#pragma once

#include <PhotonMath.h>

namespace Photon {
    namespace Math {

        template<typename T>
        class Matrix4x4 {
        public:
            T m[4][4];

            Matrix4x4();
            Matrix4x4(T mat[4][4]);
            Matrix4x4(T m00, T m01, T m02, T m03,
                      T m10, T m11, T m12, T m13,
                      T m20, T m21, T m22, T m23,
                      T m30, T m31, T m32, T m33);

            Matrix4x4<T> mul(const Matrix4x4& mat) const;
            Matrix4x4<T> operator*(T scalar) const;
            bool invert(Matrix4x4<T>& ret) const;
        };

        template<typename T>
        Matrix4x4<T> operator*(T scalar, const Matrix4x4<T>& mat);

        template<typename T>
        Matrix4x4<T> mul(const Matrix4x4<T>& mat1, const Matrix4x4<T>& mat2);

        template<typename T>
        Matrix4x4<T> transpose(const Matrix4x4<T>& mat);

        template<typename T>
        bool swapsHandedness(const Matrix4x4<T>& mat);


        // Generic RxC Matrix
        template<uint32 R, uint32 C, typename T>
        class Matrix {
        public:
            T m[R][C];

            Matrix();
            Matrix(T mat[R][C]);

            Matrix<R, R, T> mul(const Matrix<C, R, T>& mat) const;
            Matrix<R, C, T> operator*(T scalar) const;
            bool invert(Matrix<R, C, T>& ret) const;
        };

    }

    // Matrix types definition
    typedef Math::Matrix4x4<Float> Mat4;
}

#include <Matrix.inl>