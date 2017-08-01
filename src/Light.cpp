#include <Light.h>

#include <PhotonMath.h>
#include <Ray.h>

using namespace Photon;

void Light::initialize(const Scene& scene) {

}

void Light::setIntensity(const Color& Le) {
    _Le = Le;
}

const Color& Light::emission() const {
    return _Le;
}

Color Light::L(const RayEvent& evt, const Vec3& w) const {
    if (Math::dot(evt.normal, w) > 0)
        return _Le;

    return Color(0);
}

uint32 Light::numSamples() const {
    return 1;
}

bool Light::isEnvironment() const {
    return false;
}

bool Light::isDelta() const {
    return false;
}