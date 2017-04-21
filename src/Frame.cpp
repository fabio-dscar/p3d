#include <Frame.h>

using namespace Photon;

Frame::Frame() : _x(0), _y(0), _z(0) { }

Frame::Frame(const Normal& normal) : _z(normalize(Vec3(normal))) {
    Math::basisFromVector(_z, &_x, &_y);
    _x.normalize();
    _y.normalize();

    if (!consistent())
        std::cout << "WARNING";
}

Frame::Frame(const Vec3& x, const Vec3& y, const Vec3& z) 
    : _x(x), _y(y), _z(z) { 

    _x.normalize();
    _y.normalize();
    _z.normalize();
}

Vec3 Frame::toWorld(const Vec3& vec) const {
    return _x * vec.x + _y * vec.y + _z * vec.z;
}

Vec3 Frame::toLocal(const Vec3& vec) const {
    Float dotU = dot(_x, vec);
    Float dotV = dot(_y, vec);
    Float dotN = dot(_z, vec);

    return Vec3(dotU, dotV, dotN);
}

bool Frame::consistent() {
    Float lenX = _x.length();
    Float lenY = _y.length();
    Float lenZ = _z.length();

    if (lenX > (1 + F_EPSILON) || lenY > (1 + F_EPSILON) || lenZ > (1 + F_EPSILON))
        return false;

    Float dot12 = dot(_x, _y);
    Float dot13 = dot(_x, _z);
    Float dot23 = dot(_y, _z);
    return (abs(dot12) < F_EPSILON &&
            abs(dot13) < F_EPSILON &&
            abs(dot23) < F_EPSILON);
}

Vec3 Frame::reflect(const Vec3& wi) {
    return Vec3(-wi.x, -wi.y, wi.z);
}

Float Frame::cosTheta(const Vec3& w) {
    return w.z;
}

Float Frame::cosThetaSqr(const Vec3& w) {
    return w.z * w.z;
}

Float Frame::absCosTheta(const Vec3& w) {
    return std::abs(w.z);
}

Float Frame::sinTheta(const Vec3& w) {
    return std::sqrt(sinThetaSqr(w));
}

Float Frame::sinThetaSqr(const Vec3& w) {
    return std::max(0.0, 1.0 - cosThetaSqr(w));
}

Float Frame::tanTheta(const Vec3& w) {
    return sinTheta(w) / cosTheta(w);
}

Float Frame::tanThetaSqr(const Vec3& w) {
    return sinThetaSqr(w) / cosThetaSqr(w);
}

Float Frame::cosPhi(const Vec3& w) {
    Float sin = sinTheta(w);
    if (sin == 0)
        return 1;

    return Math::clamp<Float>(w.x / sin, -1.0, 1.0);
}

Float Frame::sinPhi(const Vec3& w) {
    Float sin = sinTheta(w);
    if (sin == 0)
        return 1;

    return Math::clamp<Float>(w.x / sin, -1.0, 1.0);
}

Float Frame::cosPhiSqr(const Vec3& w) {
    Float cos = cosPhi(w);
    return cos * cos;
}

Float Frame::sinPhiSqr(const Vec3& w) {
    Float sin = sinPhi(w);
    return sin * sin;
}

Float Frame::cosAng(const Vec3& w1, const Vec3& w2) {
    // Project w1 and w2
    Vec2 a = Vec2(w1.x, w1.y);
    Vec2 b = Vec2(w2.x, w2.y);

    // Use dot product definition
    return Math::clamp(Math::dot(a, b) / (a.length() * b.length()), -1, 1);
}