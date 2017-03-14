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

float Ray::getTime(const Vec3& point) const {
    return glm::length(point - _origin);
}

float Ray::getFarTime() const {
    return _farT;
}

float Ray::getNearTime() const {
    return _nearT;
}

bool Ray::isValidTime(float t) const {
    return t > _nearT && t < _farT;
}

Ray Ray::reflect(const HitInfo& info) const {
    // Calculate reflected vector
    float NdotD = glm::dot(info._normal, -_dir);
    Vec3 r = glm::normalize(2.0f * NdotD * info._normal - (-_dir));

    return Ray(info._point, r);
}

Ray Ray::refract(const HitInfo& info, float inIOR) const {
    // Calculate refracted vector
    /*Vec3 vt = glm::dot(-_dir, info._normal) * info._normal - (-_dir);
    float sinIn = glm::length(vt);
    float sinTr = (inIOR / info._obj->getMaterial().getIor()) * sinIn;
    float cosTr = std::sqrt(1.0f - (sinTr * sinTr));
    Vec3 t = sinTr * glm::normalize(vt) + cosTr * (-info._normal);
    */

    /*float cosThetaI = glm::dot(info._normal, -_dir);
    float eta = info._obj->getMaterial().getIor();

    /*if (info._backface)
        eta = 1.0f / eta;
    */

    /*float temp = std::sqrt(1.0f - (1.0f - cosThetaI * cosThetaI) / (eta * eta));

    Vec3 vt = glm::normalize(-_dir / eta - (temp - cosThetaI / eta) * info._normal);
    */

    /*float cosI = glm::dot(info._normal, -_dir);
    float cosT = std::sqrt(1.0 - 1.0 / (eta * eta) * (1.0 - cosI * cosI));
    Vec3 t = 1.0f / eta * (-_dir) - (cosT - 1.0f / eta * cosI) * info._normal;
    */

    /*float eta = 1.0f / info._obj->getMaterial().getIor();

    if (info._backface)
        eta = 1.0f / eta;

    Vec3 t = eta * (-_dir);
    float NdotI = glm::dot(info._normal, -_dir);
    float temp = eta * NdotI - std::sqrt(1.0f - (eta * eta) * (1.0f - NdotI * NdotI));

    Vec3 rt = t + info._normal * temp;

    return Ray(info._point, rt);

    */

    float eta = 1.0f / info._obj->getMaterial().getIor();
    if (info._backface)
        eta = 1.0f / eta;

    Vec3 vt = glm::dot(-_dir, info._normal) * info._normal - (-_dir);
    float sinIn = glm::length(vt);
    float sinTr = eta * sinIn;
    float cosTr = std::sqrt(1.0f - (sinTr * sinTr));
    Vec3 t = sinTr * glm::normalize(vt) + cosTr * (-info._normal);
    
    return Ray(info._point, glm::normalize(t));
}