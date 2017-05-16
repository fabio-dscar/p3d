#include <Bounds.h>

#include <Ray.h>
#include <Sphere.h>

using namespace Photon;

const Bounds3 Bounds3::UNBOUNDED = Bounds3();

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
    Float tmin = ray.minT();
    Float tmax = ray.maxT();

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

Vec3 Bounds3::sizes() const {
    return abs(_max - _min);
}

Point3 Bounds3::center() const {
    return (Float)0.5 * (_max + _min);
}

Float Bounds3::volume() const {
    Vec3 len = sizes();
    return len.cube();
}

Sphere Bounds3::sphere() const {
    Point3 pos = center();
    Float radius = dist(_max, pos);
    return Sphere(pos, radius + F_EPSILON);
}

bool Bounds3::overlaps(const Bounds3& box) const {
    return (_max.x >= box[0].x) && (_min.x <= box[1].x) &&
           (_max.y >= box[0].y) && (_min.y <= box[1].y) &&
           (_max.z >= box[0].z) && (_min.z <= box[1].z);
}

bool Bounds3::isBounded() const {
    return !(_min.infinity() || _max.infinity());
}

void Bounds3::expand(Float size) {
    _min = _min + Point3(-size);
    _max = _max + Point3(size);
}

void Bounds3::expand(const Point3& pt) {
    _min = Math::min(_min, pt);
    _max = Math::max(_max, pt);
}

void Bounds3::expand(const Bounds3& box) {
    _min = Math::min(_min, box[0]);
    _max = Math::max(_max, box[1]);
}

void Bounds3::intersect(const Bounds3& box) {
    _min = Math::max(_min, box[0]);
    _max = Math::min(_max, box[1]);
}



const Bounds2 Bounds2::UNBOUNDED = Bounds2();

const Point2& Bounds2::min() const {
    return _min;
}

const Point2& Bounds2::max() const {
    return _max;
}

const Point2& Bounds2::operator[](uint32 i) const {
    if (i == 0)
        return _min;

    return _max;
}

Vec2 Bounds2::sizes() const {
    return abs(_max - _min);
}

Point2 Bounds2::center() const {
    return (Float)0.5 * (_max + _min);
}

Float Bounds2::area() const {
    Vec2 len = sizes();
    return len.x * len.y;
}

bool Bounds2::contains(const Point2& pos) const {
    return (pos.x <= _max.x && pos.x >= _min.x) &&
           (pos.y <= _max.y && pos.y >= _min.y);
}

bool Bounds2::overlaps(const Bounds2& box) const {
    return (_max.x >= box[0].x) && (_min.x <= box[1].x) &&
           (_max.y >= box[0].y) && (_min.y <= box[1].y);
}

void Bounds2::expand(Float size) {
    _min = _min + Point3(-size);
    _max = _max + Point3(size);
}

void Bounds2::expand(const Point2& pt) {
    _min = Math::min(_min, pt);
    _max = Math::max(_max, pt);
}

void Bounds2::expand(const Bounds2& box) {
    _min = Math::min(_min, box[0]);
    _max = Math::max(_max, box[1]);
}

void Bounds2::intersect(const Bounds2& box) {
    _min = Math::max(_min, box[0]);
    _max = Math::min(_max, box[1]);
}

bool Bounds2::isBounded() const {
    return !(_min.infinity() || _max.infinity());
}