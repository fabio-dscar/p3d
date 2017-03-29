#include <Vector.h>

using namespace Photon::Math;

template<typename T> 
Vector3<T> Vector3<T>::operator+(const Vector3<T>& vec) const {
    return Vector3(x + vec.x, y + vec.y, z + vec.z);
}

/*
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

};*/