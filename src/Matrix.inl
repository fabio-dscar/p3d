namespace Photon {
    namespace Math {

        /* ----------------------------------------------------------
            Matrix4x4 member functions
        ---------------------------------------------------------*/
        template<typename T>
        Matrix4x4<T>::Matrix4x4() {
            // Set up identity
            m[0][0] = 1; m[1][0] = 0; m[2][0] = 0; m[3][0] = 0;
            m[0][1] = 0; m[1][1] = 1; m[2][1] = 0; m[3][1] = 0;
            m[0][2] = 0; m[1][2] = 0; m[2][2] = 1; m[3][2] = 0;
            m[0][3] = 0; m[1][3] = 0; m[2][3] = 0; m[3][3] = 1;
        }

        template<typename T>
        Matrix4x4<T>::Matrix4x4(T mat[4][4]) {
            m[0][0] = mat[0][0]; m[1][0] = mat[1][0]; m[2][0] = mat[2][0]; m[3][0] = mat[3][0];
            m[0][1] = mat[0][1]; m[1][1] = mat[1][1]; m[2][1] = mat[2][1]; m[3][1] = mat[3][1];
            m[0][2] = mat[0][2]; m[1][2] = mat[1][2]; m[2][2] = mat[2][2]; m[3][2] = mat[3][2];
            m[0][3] = mat[0][3]; m[1][3] = mat[1][3]; m[2][3] = mat[2][3]; m[3][3] = mat[3][3];
        }

        template<typename T>
        Matrix4x4<T>::Matrix4x4(T m00, T m01, T m02, T m03,
                                T m10, T m11, T m12, T m13,
                                T m20, T m21, T m22, T m23,
                                T m30, T m31, T m32, T m33) {

            // Row major
            m[0][0] = m00; m[1][0] = m10; m[2][0] = m20; m[3][0] = m30;
            m[0][1] = m01; m[1][1] = m11; m[2][1] = m21; m[3][1] = m31;
            m[0][2] = m02; m[1][2] = m12; m[2][2] = m22; m[3][2] = m32;
            m[0][3] = m03; m[1][3] = m13; m[2][3] = m23; m[3][3] = m33;
        }

        template<typename T>
        T& Matrix4x4<T>::operator()(uint32 i, uint32 j) {
            return m[i][j];
        }

        template<typename T>
        const T& Matrix4x4<T>::operator()(uint32 i, uint32 j) const {
            return m[i][j];
        }

        template<typename T>
        Matrix4x4<T> Matrix4x4<T>::mul(const Matrix4x4& mat) const {
            Matrix4x4<T> ret;
            for (uint32 i = 0; i < 4; ++i)
                for (uint32 j = 0; j < 4; ++j)
                    ret[i][j] = m[i][0] * mat.m[0][j] +
                    m[i][1] * mat.m[1][j] +
                    m[i][2] * mat.m[2][j] +
                    m[i][3] * mat.m[3][j];
            return ret;
        }

        template<typename T>
        Matrix4x4<T> Matrix4x4<T>::operator*(T scalar) const {
            Matrix4x4<t> ret;
            for (uint32 i = 0; i < 4; ++i)
                for (uint32 j = 0; j < 4; ++j)
                    ret[i][j] = m[i][j] * scalar;
            return ret;
        }

        template<typename T>
        bool Matrix4x4<T>::invert(Matrix4x4<T>& ret) const {
            int indxc[4], indxr[4];
            int ipiv[4];
            memset(ipiv, 0, sizeof(int) * 4);
            memcpy(ret.m, m, 16 * sizeof(T));

            for (int i = 0; i < 4; i++) {
                int irow = -1, icol = -1;
                T big = 0;
                for (int j = 0; j < 4; j++) {
                    if (ipiv[j] != 1) {
                        for (int k = 0; k < 4; k++) {
                            if (ipiv[k] == 0) {
                                if (std::abs(ret.m[j][k]) >= big) {
                                    big = std::abs(ret.m[j][k]);
                                    irow = j;
                                    icol = k;
                                }
                            } else if (ipiv[k] > 1) {
                                return false;
                            }
                        }
                    }
                }
                ++ipiv[icol];
                if (irow != icol) {
                    for (int k = 0; k < 4; ++k)
                        std::swap(ret.m[irow][k], ret.m[icol][k]);
                }
                indxr[i] = irow;
                indxc[i] = icol;
                if (ret.m[icol][icol] == 0)
                    return false;
                T pivinv = 1.0 / ret.m[icol][icol];
                ret.m[icol][icol] = 1.f;
                for (int j = 0; j < 4; j++)
                    ret.m[icol][j] *= pivinv;
                for (int j = 0; j < 4; j++) {
                    if (j != icol) {
                        T save = ret.m[j][icol];
                        ret.m[j][icol] = 0;
                        for (int k = 0; k < 4; k++)
                            ret.m[j][k] -= ret.m[icol][k] * save;
                    }
                }
            }
            for (int j = 4 - 1; j >= 0; j--) {
                if (indxr[j] != indxc[j]) {
                    for (int k = 0; k < 4; k++)
                        std::swap(ret.m[k][indxr[j]], ret.m[k][indxc[j]]);
                }
            }

            return true;
        }

        /* ----------------------------------------------------------
            Matrix4x4 functions
        ---------------------------------------------------------*/
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
            mat.invert(ret);
            return ret;
        }

        template<typename T>
        T det3x3(const Matrix4x4<T>& mat) {
            return mat.m[0][0] * (mat.m[1][1] * mat.m[2][2] - mat.m[1][2] * mat.m[2][1]) +
                   mat.m[0][1] * (mat.m[1][2] * mat.m[2][0] - mat.m[1][0] * mat.m[2][2]) +
                   mat.m[0][2] * (mat.m[1][0] * mat.m[2][1] - mat.m[1][1] * mat.m[2][0]);
        }


        /* ----------------------------------------------------------
            Matrix member functions
        ---------------------------------------------------------*/
        template<uint32 R, uint32 C, typename T>
        Matrix<R, C, T>::Matrix() {
            for (uint32 i = 0; i < L; ++i)
                for (uint32 j = 0; j < C; ++j)
                    m[i][j] = 0;
        }

        template<uint32 R, uint32 C, typename T>
        Matrix<R, C, T>::Matrix(T mat[R][C]) {
            for (uint32 i = 0; i < L; ++i)
                for (uint32 j = 0; j < C; ++j)
                    m[i][j] = mat[i][j];
        }

        template<uint32 R, uint32 C, typename T>
        Matrix<R, R, T> Matrix<R, C, T>::mul(const Matrix<C, R, T>& mat) const {
            Matrix<R, R, T> ret;

            for (uint32 ri = 0; ri < R; ++ri)
                for (uint32 rj = 0; rj < R; ++rj)
                    for (uint32 j = 0; j < C; ++j)
                        ret.m[ri][rj] += m[ri][j] * mat.m[j][rj];

            return ret;
        }

        template<uint32 R, uint32 C, typename T>
        Matrix<R, C, T> Matrix<R, C, T>::operator*(T scalar) const {
            Matrix<R, C, T> ret;

            for (uint32 i = 0; i < R; ++i)
                for (uint32 j = 0; j < C; ++j)
                    ret.m[i][j] = m[i][j] * scalar;

            return ret;
        }

        template<uint32 R, uint32 C, typename T>
        bool Matrix<R, C, T>::invert(Matrix<R, C, T>& ret) const {
            if (L != C)
                return false;

            int indxc[C], indxr[C];
            int ipiv[C];
            memset(ipiv, 0, sizeof(int) * C);
            memcpy(ret.m, m, R * C * sizeof(T));

            for (int i = 0; i < C; i++) {
                int irow = -1, icol = -1;
                T big = 0;
                for (int j = 0; j < C; j++) {
                    if (ipiv[j] != 1) {
                        for (int k = 0; k < C; k++) {
                            if (ipiv[k] == 0) {
                                if (std::abs(ret.m[j][k]) >= big) {
                                    big = std::abs(ret.m[j][k]);
                                    irow = j;
                                    icol = k;
                                }
                            } else if (ipiv[k] > 1) {
                                return false;
                            }
                        }
                    }
                }
                ++ipiv[icol];
                if (irow != icol) {
                    for (int k = 0; k < C; ++k)
                        std::swap(ret.m[irow][k], ret.m[icol][k]);
                }
                indxr[i] = irow;
                indxc[i] = icol;

                if (ret.m[icol][icol] == 0)
                    return false;

                T pivinv = 1.0 / ret.m[icol][icol];
                ret.m[icol][icol] = 1.f;
                for (int j = 0; j < C; j++)
                    ret.m[icol][j] *= pivinv;
                for (int j = 0; j < C; j++) {
                    if (j != icol) {
                        T save = ret.m[j][icol];
                        ret.m[j][icol] = 0;
                        for (int k = 0; k < C; k++)
                            ret.m[j][k] -= ret.m[icol][k] * save;
                    }
                }
            }

            for (int j = C - 1; j >= 0; j--) {
                if (indxr[j] != indxc[j]) {
                    for (int k = 0; k < C; k++)
                        std::swap(ret.m[k][indxr[j]], ret.m[k][indxc[j]]);
                }
            }

            return true;
        }
       
    }
}