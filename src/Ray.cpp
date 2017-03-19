#include <Ray.h>
#include <Material.h>
#include <Geometry.h>

using namespace Photon;

const Vec3& Ray::origin() const {
    return _origin;
}

const Vec3& Ray::dir() const {
    return _dir;
}

float Ray::arg(const Vec3& point) const {
    return glm::length(point - _origin);
}

void Photon::Ray::setMaxT(float t) const {
    _maxT = t;
}

void Ray::setPrimary(bool isPrimary) {
    _isPrimary = isPrimary;
}

float Ray::farT() const {
    return _maxT;
}

float Ray::nearT() const {
    return _minT;
}

Vec3 Ray::hitPoint() const {
    return _origin + _maxT * _dir;
}

bool Ray::inRange(float t) const {
    return t > _minT && t < _maxT;
}

bool Ray::isPrimary() const {
    return _isPrimary;
}

Ray Ray::reflect(const SurfaceEvent& evt) const {
    // Calculate reflected vector
    float NdotD = glm::dot(evt.normal(), -_dir);
    Vec3 r = glm::normalize(2.0f * NdotD * evt.normal() - (-_dir));

    return Ray(evt.point(), r);
}

Ray Ray::refract(const SurfaceEvent& evt, float ior) const {
    float eta = ior / evt.obj()->getMaterial().getIor();
    if (evt.backface()) // Check if we are leaving the material
        eta = ior / eta;

    Vec3 vt = glm::dot(-_dir, evt.normal()) * evt.normal() - (-_dir);
    float sinIn = glm::length(vt);
    float sinTr = eta * sinIn;

    // If total internal reflection
    if ((sinTr * sinTr) > 1.0f) 
        return reflect(evt);  // Return reflection instead

    float cosTr = std::sqrt(1.0f - (sinTr * sinTr));

    // Calculate refracted direction
    Vec3 t = sinTr * glm::normalize(vt) + cosTr * (-evt.normal());
    
    return Ray(evt.point(), glm::normalize(t));
}

Vec3 Ray::operator()(float t) const {
    return _origin + t * _dir;
}