#include <Light.h>

#include <PhotonMath.h>
#include <Ray.h>

using namespace Photon;

void Light::setIntensity(const Color& Le) {
    _Le = Le;
}

const Color& Light::color() const {
    return _Le;
}

Color Light::L(const RayEvent& evt, const Vec3& w) const {
    if (Math::dot(evt.normal, w) > 0)
        return _Le;

    return Color(0);
}


bool Light::isEnvironment() const {
    return false;
}

bool Light::isDelta() const {
    return false;
}