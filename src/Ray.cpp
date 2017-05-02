
#include <Ray.h>
#include <Material.h>
#include <Shape.h>

using namespace Photon;

/* ----------------------------------------------------------
    Ray member functions
---------------------------------------------------------*/

Ray::Ray(const Point3& origin, const Vec3& direction)
    : _origin(origin), _dir(direction), _time(0), _minT(F_RAY_OFFSET), _maxT(F_INFINITY), _isPrimary(false) {}

Ray::Ray(const Point3& origin, const Vec3& direction, Float minT, Float maxT)
    : _origin(origin), _dir(direction), _time(0), _minT(minT), _maxT(maxT), _isPrimary(false) {}

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

Float Ray::time() const {
    return _time;
}

Float Ray::arg(const Point3& point) const {
    return (point - _origin).length();
}

void Ray::setMaxT(Float t) const {
    _maxT = t;
}

void Ray::setPrimary(bool isPrimary) {
    _isPrimary = isPrimary;
}

void Ray::setTime(Float time) {
    _time = time;
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

/*Ray Ray::reflect(const SurfaceEvent& evt) const {
    // Calculate reflected vector
    Float NdotD = dot(-_dir, evt.normal);
    Vec3 r = normalize(2 * NdotD * evt.normal - (-_dir));

    return Ray(evt.point, r);
}

Ray Ray::refract(const SurfaceEvent& evt, Float ior) const {
    Float eta = ior / evt.obj->getMaterial().getIor();

    if (evt.backface) // Check if we are leaving the material
        eta = ior / eta;

    Vec3 vt = dot(-_dir, evt.normal) * evt.normal - (-_dir);
    Float sinIn = vt.length();
    Float sinTr = eta * sinIn;

    // If total internal reflection
    if ((sinTr * sinTr) > 1.0f)
        return reflect(evt);

    Float cosTr = std::sqrt(1.0f - (sinTr * sinTr));

    // Calculate refracted direction
    Vec3 t = sinTr * normalize(vt) + cosTr * -evt.normal;
    
    return Ray(evt.point, normalize(t));
}*/

Point3 Ray::operator()(Float t) const {
    return _origin + t * _dir;
}


/* ----------------------------------------------------------
    RayEvent member functions
---------------------------------------------------------*/

RayEvent::RayEvent(const Ray& ray)
    : point(ray.hitPoint()), wo(-ray.dir()), normal(0) {}

RayEvent::RayEvent(const Point3& point, const Normal& n)
    : point(point), normal(n) {}

RayEvent::RayEvent(const Point3& point, const Normal& n, const Vec3& wo)
    : point(point), normal(n), wo(wo) {}

Ray RayEvent::spawnRay(const Point3& target) const {
    return Ray(point, target);
}

Ray RayEvent::spawnRay(const Vec3& dir) const {
    return Ray(point, dir);
}

Ray RayEvent::spawnRay(const Vec3& dir, Float dist) const {
    return Ray(point, dir, F_RAY_OFFSET, dist);
}

/* ----------------------------------------------------------
    SurfaceEvent member functions
---------------------------------------------------------*/

#include <AreaLight.h>

void SurfaceEvent::setEvent(const Ray& ray, Shape const* shape, const Normal& n) {
    obj    = shape;
    point  = ray.hitPoint();
    normal = n;
    gFrame = Frame(normal); 
    wo     = gFrame.toLocal(-ray.dir());
    sFrame = gFrame;
    backface = false;
/*    sFrame = gFrame;   // Use shading normal instead
    wo     = sFrame.toLocal(-ray.dir());*/
}

bool SurfaceEvent::hit() const {
    return obj != nullptr;
}

Color SurfaceEvent::emission(const Vec3& w) const {
    // If surface is emissive, return emission
    if (obj->isLight())
        return obj->areaLight()->evalL(*this, w);

    return Color::BLACK;
}

Vec3 SurfaceEvent::toWorld(const Vec3& w) const {
    return sFrame.toWorld(w);
}

Vec3 SurfaceEvent::toLocal(const Vec3& w) const {
    return sFrame.toLocal(w);
}