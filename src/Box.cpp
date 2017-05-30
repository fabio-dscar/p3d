#include <Box.h>

using namespace Photon;

Box::Box(const Point3& min, const Point3& max)
    : _min(min), _max(max) {}

Box::Box(const Transform& objToWorld) : Shape(objToWorld) {
    _min = _objToWorld(Point3(-0.5, -0.5, -0.5));
    _max = _objToWorld(Point3(0.5, 0.5, 0.5));
}

Bounds3 Box::bbox() const {
    Bounds3 box = Bounds3(_min, _max);
    box.expand(F_EPSILON);

    return box;
}

Point3 Box::center() const {
    return (Float)0.5 * (_max + _min);
}

/*bool Box::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
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
}*/

bool Box::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    Float tmin = -F_INFINITY, tmax = F_INFINITY;

    const Ray objRay = _worldToObj(ray);

    const Point3 min = Point3(-0.5, -0.5, -0.5);
    const Point3 max = Point3(0.5, 0.5, 0.5);

    const Vec3& rayDir = objRay.dir();
    const Point3& rayOrigin = objRay.origin();

    for (int32 i = 0; i < 3; ++i) {
        Float invDir = 1.0 / rayDir[i];

        // Compute intersections and sort
        Float tNear = (min[i] - rayOrigin[i]) * invDir;
        Float tFar  = (max[i] - rayOrigin[i]) * invDir;
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
        Normal normal = calcNormal(objRay(tmin));
        evt->setEvent(ray, this, normalize(normal));

        return true;
    }

    if (ray.inRange(tmax)) {
        ray.setMaxT(tmax);
        Normal normal = calcNormal(objRay(tmax));
        evt->setEvent(ray, this, -normalize(normal));
        evt->backface = true;

        return true;
    }

    return false;
}

bool Box::isOccluded(const Ray& ray) const {
    Float tmin = ray.minT();
    Float tmax = ray.maxT();

    const Ray objRay = _worldToObj(ray);

    const Point3 min = Point3(-0.5, -0.5, -0.5);
    const Point3 max = Point3(0.5, 0.5, 0.5);

    const Vec3& rayDir = objRay.dir();
    const Point3& rayOrigin = objRay.origin();

    for (int32 i = 0; i < 3; ++i) {
        Float invDir = 1.0 / rayDir[i];

        // Compute intersections and sort
        Float tNear = (min[i] - rayOrigin[i]) * invDir;
        Float tFar  = (max[i] - rayOrigin[i]) * invDir;
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

Normal Box::calcNormal(const Point3& pt) const {
    uint32 dim = (abs(pt) - Vec3(1, 1, 1)).maxDim();

    Normal n(0);
    n[dim] = sign(pt[dim]);

    return normalize(n);

    /*Point3 d = abs(Point3(_max.x - _min.x, _max.y - _min.y, _max.z - _min.z) * 0.5);
    Float bias = 1 + F_EPSILON;

    return Normal(int32((p.x / d.x) * bias), int32((p.y / d.y) * bias), int32((p.z / d.z) * bias));*/
}