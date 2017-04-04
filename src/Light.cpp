#include <Light.h>

#include <PhotonMath.h>
#include <Ray.h>

using namespace Photon;

void Light::setPosition(const Point3& pos) {
    _pos = pos;
}

const Point3& Light::pos() const {
    return _pos;
}

void Light::setColor(const Color3& color) {
    _Le = color;
}

const Color3& Light::color() const {
    return _Le;
}

Color3 Light::L(const RayEvent& evt, const Vec3& w) const {
    if (Math::dot(evt.normal(), w) > 0)
        return _Le;

    return Color3(0);
}