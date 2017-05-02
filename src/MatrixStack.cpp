#include <MatrixStack.h>

using namespace Photon;

void MatrixStack::pushMatrix() {
    if (_matStack.size() < MAX_NUM_MATRICES)
        _matStack.emplace_back(_currMat);
}

void MatrixStack::popMatrix() {
    if (_matStack.size() > 0) {
        _currMat = _matStack[_matStack.size() - 1];
        _matStack.pop_back();
    }
}

void MatrixStack::loadIdentity() {
    _currMat = Mat4();
}

const Mat4& MatrixStack::loadMatrix() const {
    return _currMat;
}

void MatrixStack::scale(const Vec3& scale) {
    const Mat4 mat(scale.x, 0, 0, 0,
                   0, scale.y, 0, 0,
                   0, 0, scale.z, 0,
                   0, 0, 0, 1);

    _currMat = mul(_currMat, mat);
}

void MatrixStack::translate(const Vec3& trans) {
    const Mat4 mat(1, 0, 0, trans.x,
                   0, 1, 0, trans.y,
                   0, 0, 1, trans.z,
                   0, 0, 0, 1);

    _currMat = mul(_currMat, mat);
}

void MatrixStack::rotateX(Float angle) {
    Float rads = radians(angle);
    Float sin = std::sin(rads);
    Float cos = std::cos(rads);

    Mat4 mat(1, 0, 0, 0,
             0, cos, -sin, 0,
             0, sin, cos, 0,
             0, 0, 0, 1);

    _currMat = mul(_currMat, mat);
}

void MatrixStack::rotateY(Float angle) {
    Float rads = radians(angle);
    Float sin = std::sin(rads);
    Float cos = std::cos(rads);

    Mat4 mat(cos, 0, sin, 0,
             0, 1, 0, 0,
             -sin, 0, cos, 0,
             0, 0, 0, 1);

    _currMat = mul(_currMat, mat);
}

void MatrixStack::rotateZ(Float angle) {
    Float rads = radians(angle);
    Float sin = std::sin(rads);
    Float cos = std::cos(rads);

    Mat4 mat(cos, -sin, 0, 0,
             sin, cos, 0, 0,
             0, 0, 1, 0,
             0, 0, 0, 1);

    _currMat = mul(_currMat, mat);
}

void MatrixStack::setMat(const Mat4& mat) {
    _currMat = mat;
}

void MatrixStack::mulMat(const Mat4& mat) {
    _currMat = mul(_currMat, mat);
}