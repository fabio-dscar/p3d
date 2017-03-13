#include <Plane.h>

using namespace Photon;

const Vec3& Plane::getNormal() const {
    return _normal;
}

float Plane::getDistance() const {
    return _distance;
}

bool Plane::intersectRay(const Ray& ray, HitInfo& info) const {
    float NdotD = glm::dot(_normal, ray.getDirection());
    if (std::abs(NdotD) > 1e-6) {
        float NdotO = glm::dot(_normal, ray.getOrigin());

        info._t = (_distance - NdotO) / NdotD;
        info._hit = ray.isValidTime(info._t);
        info._point = ray.getPoint(info._t);
        info._normal = _normal;
        info._obj = (Geometry*)this;

        return info._hit;
    }

    return false;
}