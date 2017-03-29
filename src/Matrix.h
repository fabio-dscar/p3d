#pragma once

#include <MathDefs.h>

namespace Photon {

namespace Math {

    template<typename T>
    class Matrix4x4 {
    public:
        Float m[4][4];
        
        Matrix4x4() { 
            // Set up identity
            m[0][0] = 1; m[1][0] = 0; m[2][0] = 0; m[3][0] = 0;
            m[0][1] = 0; m[1][1] = 1; m[2][1] = 0; m[3][1] = 0;
            m[0][2] = 0; m[1][2] = 0; m[2][2] = 1; m[3][2] = 0;
            m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
        }

        Matrix4x4(T mat[4][4]) {           
            m[0][0] = mat[0][0]; m[1][0] = mat[1][0]; m[2][0] = mat[2][0]; m[3][0] = mat[3][0];
            m[0][1] = mat[0][1]; m[1][1] = mat[1][1]; m[2][1] = mat[2][1]; m[3][1] = mat[3][1];
            m[0][2] = mat[0][2]; m[1][2] = mat[1][2]; m[2][2] = mat[2][2]; m[3][2] = mat[3][2];
            m[0][3] = mat[0][3]; m[1][3] = mat[1][3]; m[2][3] = mat[2][3]; m[3][3] = mat[3][3];
        }

        Matrix4x4(T m00, T m01, T m02, T m03,
                  T m10, T m11, T m12, T m13,
                  T m20, T m21, T m22, T m23,
                  T m30, T m31, T m32, T m33) {

            // Row major
            m[0][0] = m00; m[1][0] = m10; m[2][0] = m20; m[3][0] = m30;
            m[0][1] = m01; m[1][1] = m11; m[2][1] = m21; m[3][1] = m31;
            m[0][2] = m02; m[1][2] = m12; m[2][2] = m22; m[3][2] = m32;
            m[0][3] = m03; m[1][3] = m13; m[2][3] = m23; m[3][3] = m33;
        }

        Matrix4x4<T> mul(const Matrix4x4& mat) const {
            Matrix4x4<T> ret;
            for (uint32 i = 0; i < 4; ++i)
                for (uint32 j = 0; j < 4; ++j)
                    ret[i][j] = m[i][0] * mat.m[0][j] +
                                m[i][1] * mat.m[1][j] +
                                m[i][2] * mat.m[2][j] +
                                m[i][3] * mat.m[3][j];
            return ret;        
        }

        Matrix4x4<T> operator*(T scalar) const {
            Matrix4x4<t> ret;
            for (uint32 i = 0; i < 4; ++i)
                for (uint32 j = 0; j < 4; ++j)
                    ret[i][j] = m[i][j] * scalar;
            return ret;
        }

    };

    template<typename T>
    Matrix4x4<T> operator*(T scalar, const Matrix4x4<T>& mat) {
        return mat * scalar;
    }

    template<typename T>
    Matrix4x4<T> mul(const Matrix4x4<T>& mat1, const Matrix4x4<T>& mat2) {
        Matrix4x4<T> ret;
        for (uint32 i = 0; i < 4; ++i)
            for (uint32 j = 0; j < 4; ++j)
                ret.m[i][j] = mat1.m[i][0] * mat2.m[0][j] +
                            mat1.m[i][1] * mat2.m[1][j] +
                            mat1.m[i][2] * mat2.m[2][j] +
                            mat1.m[i][3] * mat2.m[3][j];
        return ret;
    }

    template<typename T>
    Matrix4x4<T> transpose(const Matrix4x4<T>& mat) {
        return Matrix4x4<T>(mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
                         mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
                         mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
                         mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
    }

    template<typename T>
    Matrix4x4<T> inverse(const Matrix4x4<T>& mat) {
        Matrix4x4<T> ret;
        // TODO
        return ret;
    }


    typedef Math::Matrix4x4<Float> Mat4;

}

}