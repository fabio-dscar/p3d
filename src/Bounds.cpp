#include <Bounds.h>

#include <Ray.h>

using namespace Photon;

const Point3& Bounds3::min() const {
    return _min;
}

const Point3& Bounds3::max() const {
    return _max;
}

bool Bounds3::contains(const Point3& pos) const {
    return (pos.x <= _max.x && pos.x >= _min.x) &&
           (pos.y <= _max.y && pos.y >= _min.y) &&
           (pos.z <= _max.z && pos.z >= _min.z);
}

bool Bounds3::intersectPts(const Ray& ray, Float* t0, Float* t1) const {
    Float tmin = ray.minT(), tmax = ray.maxT();

    const Vec3& rayDir = ray.dir();
    const Point3& rayOrigin = ray.origin();

    for (int32 i = 0; i < 3; i++) {
        Float invDir = 1.0 / rayDir[i];

        // Compute intersections and sort
        Float tNear = (_min[i] - rayOrigin[i]) * invDir;
        Float tFar  = (_max[i] - rayOrigin[i]) * invDir;
        if (tNear > tFar)
            std::swap(tNear, tFar);

        // Cut the ray's interval with this slab
        if (tNear > tmin)
            tmin = tNear;

        if (tFar < tmax)
            tmax = tFar;

        // If the interval is not valid, it does not intersect
        if (tmin > tmax)
            return false;
    }

    *t0 = tmin;
    *t1 = tmax;
    return true;
}

const Point3& Bounds3::operator[](uint32 i) const {
    if (i == 0)
        return _min;

    return _max;
}

bool Bounds3::overlaps(const Bounds3& box) const {
    return (_max.x >= box[0].x) && (_min.x <= box[1].x) &&
        (_max.y >= box[0].y) && (_min.y <= box[1].y) &&
        (_max.z >= box[0].z) && (_min.z <= box[1].z);
}

bool Bounds3::bounded() const {
    return !(_min.infinity() || _max.infinity());
}