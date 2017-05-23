#include <Box.h>

using namespace Photon;

Bounds3 Box::bbox() const {
    return Bounds3(_min + Point3(-F_EPSILON), _max + Point3(F_EPSILON));
}

Point3 Box::center() const {
    return (Float)0.5 * (_max + _min);
}

bool Box::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    Float tmin = -F_INFINITY, tmax = F_INFINITY;

    const Vec3& rayDir = ray.dir();
    const Point3& rayOrigin = ray.origin();

    for (int32 i = 0; i < 3; ++i) {
        Float invDir = 1.0 / rayDir[i];

        // Compute intersections and sort
        Float tNear = (_min[i] - rayOrigin[i]) * invDir;
        Float tFar  = (_max[i] - rayOrigin[i]) * invDir;
        if (tNear > tFar)
            std::swap(tNear, tFar);

        // Cut the ray's interval with this slab
        tmin = std::max(tmin, tNear);
        tmax = std::min(tmax, tFar);

        // If the interval is not valid, it does not intersect
        if (tmin > tmax)
            return false;
    }

    if (ray.inRange(tmin)) {
        ray.setMaxT(tmin);
        Normal normal = calcNormal(ray.hitPoint());
        evt->setEvent(ray, this, normalize(normal));

        return true;
    }

    if (ray.inRange(tmax)) {
        ray.setMaxT(tmax);
        Normal normal = calcNormal(ray.hitPoint());
        evt->setEvent(ray, this, -normalize(normal));
        evt->backface = true;

        return true;
    }

    return false;
}

bool Box::isOccluded(const Ray& ray) const {
    Float tmin = ray.minT();
    Float tmax = ray.maxT();

    const Vec3& rayDir = ray.dir();
    const Point3& rayOrigin = ray.origin();

    for (int32 i = 0; i < 3; ++i) {
        Float invDir = 1.0 / rayDir[i];

        // Compute intersections and sort
        Float tNear = (_min[i] - rayOrigin[i]) * invDir;
        Float tFar  = (_max[i] - rayOrigin[i]) * invDir;
        if (tNear > tFar)
            std::swap(tNear, tFar);

        // Cut the ray's interval with this slab
        tmin = std::max(tmin, tNear);
        tmax = std::min(tmax, tFar);

        // If the interval is not valid, it does not intersect
        if (tmin > tmax)
            return false;
    }

    return (ray.inRange(tmin) || ray.inRange(tmax));
}