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

void Light::setColor(const Color& color) {
    _Le = color;
}

const Color& Light::color() const {
    return _Le;
}

Color Light::L(const RayEvent& evt, const Vec3& w) const {
    if (Math::dot(evt.normal(), w) > 0)
        return _Le;

    return Color(0);
}


bool Light::isEnvironment() const {
    return false;
}

bool Light::isDelta() const {
    return false;
}