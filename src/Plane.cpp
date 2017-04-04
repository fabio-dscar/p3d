#include <Plane.h>

using namespace Photon;

const Normal& Plane::normal() const {
    return _normal;
}

Float Plane::distance() const {
    return _distance;
}

Bounds3 Plane::bbox() const {
    // The plane is unbounded
    return Bounds3::UNBOUNDED;
}

bool Plane::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    Float NdotD = dot(_normal, ray.dir());
    if (std::abs(NdotD) > F_EPSILON) {
        Float NdotO = dot(_normal, ray.origin().posVec());
        Float t = (_distance - NdotO) / NdotD;

        if (ray.inRange(t)) {
            ray.setMaxT(t);
            evt->setEvent(ray, this, _normal);
            return true;
        } 
    }

    return false;
}

bool Plane::isOccluded(const Ray& ray) const {
    Float NdotD = dot(_normal, ray.dir());
    if (std::abs(NdotD) > F_EPSILON) {
        Float NdotO = dot(_normal, ray.origin().posVec());
        Float t = (_distance - NdotO) / NdotD;

        if (ray.inRange(t))
            return true;
    }

    return false;
}