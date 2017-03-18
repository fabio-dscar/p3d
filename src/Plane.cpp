#include <Plane.h>

using namespace Photon;

const Vec3& Plane::normal() const {
    return _normal;
}

float Plane::distance() const {
    return _distance;
}

bool Plane::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    float NdotD = glm::dot(_normal, ray.dir());
    if (std::abs(NdotD) > F_EPSILON) {
        float NdotO = glm::dot(_normal, ray.origin());
        float t = (_distance - NdotO) / NdotD;

        if (ray.inRange(t)) {
            ray.setMaxT(t);
            evt->setEvent(ray, this, _normal);
            return true;
        } 
    }

    return false;
}

bool Plane::isOccluded(const Ray& ray) const {
    float NdotD = glm::dot(_normal, ray.dir());
    if (std::abs(NdotD) > F_EPSILON) {
        float NdotO = glm::dot(_normal, ray.origin());
        float t = (_distance - NdotO) / NdotD;

        if (ray.inRange(t))
            return true;
    }

    return false;
}