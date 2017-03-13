#include <Sphere.h>

using namespace Photon;

const Vec3& Sphere::getPosition() const {
    return _pos;
}

float Sphere::getRadius() const {
    return _radius;
}

bool Sphere::intersectRay(const Ray& ray, HitInfo& info) const {
    float dSqr = glm::length2(ray.getOrigin() - _pos);
    float radiusSqr = _radius * _radius;
    if (dSqr == radiusSqr)
        return false;

    float b = glm::dot(ray.getDirection(), _pos - ray.getOrigin());

    if (dSqr > radiusSqr)
        if (b < 0)
            return false;

    float r = b * b - dSqr + radiusSqr;
    if (r < 0)
        return false;

    float t;
    if (dSqr > radiusSqr)
        t = b - std::sqrt(r);
    else if (dSqr < radiusSqr)
        t = b + std::sqrt(r);

    info._hit = true;
    info._t = t;
    info._point = ray.getPoint(t);
    info._obj = (Geometry*)this;
    info._normal = glm::normalize((info._point - _pos) / _radius);
    if (dSqr < radiusSqr)
        info._normal = -info._normal;

    return true;
}