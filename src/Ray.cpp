#include <Ray.h>
#include <Material.h>
#include <Geometry.h>

using namespace Photon;

const Point3& Ray::origin() const {
    return _origin;
}

const Vec3& Ray::dir() const {
    return _dir;
}

Float Ray::arg(const Point3& point) const {
    return (point - _origin).length();
}

void Photon::Ray::setMaxT(Float t) const {
    _maxT = t;
}

void Ray::setPrimary(bool isPrimary) {
    _isPrimary = isPrimary;
}

Float Ray::maxT() const {
    return _maxT;
}

Float Ray::minT() const {
    return _minT;
}

Point3 Ray::hitPoint() const {
    return _origin + _maxT * _dir;
}

bool Ray::inRange(Float t) const {
    return t > _minT && t < _maxT;
}

bool Ray::isPrimary() const {
    return _isPrimary;
}

Ray Ray::reflect(const SurfaceEvent& evt) const {
    // Calculate reflected vector
    Float NdotD = dot(-_dir, evt.normal());
    Vec3 r = normalize(2.0f * NdotD * evt.normal() - (-_dir));

    return Ray(evt.point(), r);
}

Ray Ray::refract(const SurfaceEvent& evt, Float ior) const {
    Float eta = ior / evt.obj()->getMaterial().getIor();
    if (evt.backface()) // Check if we are leaving the material
        eta = ior / eta;

    Vec3 vt = dot(-_dir, evt.normal()) * evt.normal() - (-_dir);
    Float sinIn = vt.length();
    Float sinTr = eta * sinIn;

    // If total internal reflection
    if ((sinTr * sinTr) > 1.0f)
        return reflect(evt);

    Float cosTr = std::sqrt(1.0f - (sinTr * sinTr));

    // Calculate refracted direction
    Vec3 t = sinTr * normalize(vt) + cosTr * -evt.normal();
    
    return Ray(evt.point(), normalize(t));
}

Point3 Ray::operator()(Float t) const {
    return _origin + t * _dir;
}