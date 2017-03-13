#include <Ray.h>
#include <Material.h>
#include <Geometry.h>

using namespace Photon;

const Vec3& Ray::getOrigin() const {
    return _origin;
}

const Vec3& Ray::getDirection() const {
    return _dir;
}

const Vec3 Ray::getPoint(float arg) const {
    return _origin + arg * _dir;
}

float Ray::getArg(const Vec3& point) const {
    return glm::length(point - _origin);
}

Ray Ray::reflect(const HitInfo& info) const {
    // Offset slightly in normal direction
    Vec3 origin = Vec3(info._point) + (1e-4f * info._normal);

    // Calculate reflected vector
    float NdotD = glm::dot(info._normal, -_dir);
    Vec3 r = glm::normalize(2.0f * NdotD * info._normal - (-_dir));

    return Ray(origin, r);
}

Ray Ray::refract(const HitInfo& info, float inIOR) const {
    // Offset slightly in inverse normal direction
    Vec3 origin = Vec3(info._point) + (1e-4f * -info._normal);

    // Calculate refracted vector
    Vec3 vt = glm::dot(-_dir, info._normal) * info._normal - (-_dir);
    float sinIn = glm::length(vt);
    float sinTr = (inIOR / info._obj->getMaterial().getIor()) * sinIn;
    float cosTr = std::sqrt(1.0f - (sinTr * sinTr));
    Vec3 t = sinTr * glm::normalize(vt) + cosTr * (-info._normal);

    return Ray(origin, t);
}