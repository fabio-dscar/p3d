#include <Quat.h>
#include <Matrix.h>

using namespace Photon;
using namespace Photon::Math;

Quat::Quat(const Transform& transform) {

}

Quat Quat::operator+(const Quat& quat) const {
    return Quat(vec + quat.vec, w + quat.w);
}

Quat& Quat::operator+=(const Quat& quat) {
    vec += quat.vec;
    w   += quat.w;
    return *this;
}

Quat Quat::operator-(const Quat& quat) const {
    return Quat(vec - quat.vec, w - quat.w);
}

Quat& Quat::operator-=(const Quat& quat) {
    vec -= quat.vec;
    w   -= quat.w;
    return *this;
}

Quat Quat::operator*(Float scalar) const {
    return Quat(vec * scalar, w * scalar);
}

Quat& Quat::operator*=(Float scalar) {
    vec *= scalar;
    w   *= scalar;
    return *this;
}

Quat Quat::operator/(Float scalar) const {
    Float recip = 1.0 / scalar;
    return Quat(vec * recip, w * recip);
}

Quat& Quat::operator/=(Float scalar) {
    Float recip = 1.0 / scalar;
    vec /= recip;
    w   /= recip;
    return *this;
}

Transform Quat::transform() const {
    Matrix4x4<Float> mat;

    // Diagonal
    mat.m[0][0] = 1.0 - 2.0 * (vec.y * vec.y + vec.z * vec.z);
    mat.m[1][1] = 1.0 - 2.0 * (vec.x * vec.x + vec.z * vec.z);
    mat.m[2][2] = 1.0 - 2.0 * (vec.x * vec.x + vec.y * vec.y);
    mat.m[3][3] = 1.0;

    // Row 1
    mat.m[0][1] = 2.0 * (vec.x * vec.y + vec.z * w);
    mat.m[0][2] = 2.0 * (vec.x * vec.z - vec.y * w);
    mat.m[0][3] = 0;

    // Row 2
    mat.m[1][0] = 2.0 * (vec.x * vec.y - vec.z * w);
    mat.m[1][2] = 2.0 * (vec.y * vec.z + vec.x * w);
    mat.m[1][3] = 0;

    // Row 3
    mat.m[2][0] = 2.0 * (vec.x * vec.z + vec.y * w);
    mat.m[2][1] = 2.0 * (vec.y * vec.z - vec.x * w);
    mat.m[2][3] = 0;

    // Row 4
    mat.m[3][0] = 0;
    mat.m[3][1] = 0;
    mat.m[3][2] = 0;

    return Transform(transpose(mat), mat);
}