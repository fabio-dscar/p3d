
#include <Ray.h>
#include <Material.h>
#include <Shape.h>

using namespace Photon;

/* ----------------------------------------------------------
    Ray member functions
---------------------------------------------------------*/

Ray::Ray(const Point3& origin, const Vec3& direction)
    : _origin(origin), _dir(direction), _minT(F_RAY_OFFSET), _maxT(F_INFINITY), _isPrimary(false) {}

Ray::Ray(const Point3& origin, const Vec3& direction, Float minT, Float maxT)
    : _origin(origin), _dir(direction), _minT(minT), _maxT(maxT), _isPrimary(false) {}

Ray::Ray(const Point3& origin, const Point3& target)
    : _origin(origin), _minT(F_RAY_OFFSET), _isPrimary(false) {

    _dir = normalize(target - origin);
    _maxT = arg(target);
    _maxT -= _maxT * F_RAY_OFFSET;
}

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


/* ----------------------------------------------------------
    RayEvent member functions
---------------------------------------------------------*/

RayEvent::RayEvent(const Ray& ray)
    : _point(ray.hitPoint()), _wo(-ray.dir()), _normal(0) {}

RayEvent::RayEvent(const Point3& point, const Normal& normal)
    : _point(point), _normal(normal) {}

RayEvent::RayEvent(const Point3& point, const Normal& normal, const Vec3& wo)
    : _point(point), _normal(normal), _wo(wo) {}

Vec3 RayEvent::wo() const {
    return _wo;
}

Normal RayEvent::normal() const {
    return _normal;
}

Point3 RayEvent::point() const {
    return _point;
}

Ray RayEvent::spawnRay(const Point3& target) const {
    return Ray(_point, target);
}

Ray RayEvent::spawnRay(const Vec3& dir) const {
    return Ray(_point, dir);
}

Ray RayEvent::spawnRay(const Vec3& dir, Float dist) const {
    return Ray(_point, dir, F_RAY_OFFSET, dist);
}

/* ----------------------------------------------------------
    SurfaceEvent member functions
---------------------------------------------------------*/

#include <AreaLight.h>

void SurfaceEvent::setEvent(const Ray& ray, Shape const* obj, const Normal& normal) {
    _obj    = obj;
    _point  = ray.hitPoint();
    _normal = normal;
    _gFrame = Frame(_normal); 
    _sFrame = _gFrame;   // Use shading normal instead
    _wo     = _sFrame.toLocal(-ray.dir());
}

void SurfaceEvent::setPoint(const Point3& pt) {
    _point = pt;
}

void SurfaceEvent::setShadFrame(const Frame& shframe) {
    _sFrame = shframe;
}

void SurfaceEvent::setGeoFrame(const Frame& gframe) {
    _gFrame = gframe;
}

const Frame& SurfaceEvent::gFrame() const {
    return _gFrame;
}

const Frame& SurfaceEvent::sFrame() const {
    return _sFrame;
}

void SurfaceEvent::setObj(const Shape* obj) {
    _obj = obj;
}

void SurfaceEvent::setUV(const Point2& uv) {
    _uv = uv;
}

void SurfaceEvent::setBackface(bool backface) {
    _backface = backface;
}

Shape const* SurfaceEvent::obj() const {
    return _obj;
}

const Point2& SurfaceEvent::uv() const {
    return _uv;
}

bool SurfaceEvent::hit() const {
    return _obj != nullptr;
}

bool SurfaceEvent::backface() const {
    return _backface;
}

Color SurfaceEvent::Le(const Vec3& w) const {
    // If surface is emissive, return emission
    if (_obj->isLight())
        return _obj->areaLight()->evalL(*this, w);

    return Color::BLACK;
}

Vec3 SurfaceEvent::toWorld(const Vec3& w) const {
    return _sFrame.toWorld(w);
}

Vec3 SurfaceEvent::toLocal(const Vec3& w) const {
    return _sFrame.toLocal(w);
}