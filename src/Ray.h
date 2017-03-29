#pragma once

#include <Vector.h>
#include <MathDefs.h>

namespace Photon {

    // Forward declarations
    class Geometry;
    class SurfaceEvent;

    // This class represents a Ray parameterized by:
    //
    //      ray(_t) = _origin + _t * _dir,    with _t in [_minT, _maxT] 
    // 
    class Ray {
    private:
        Point3        _origin;
        Vec3          _dir;       
        bool          _isPrimary;
        Float         _minT;
        mutable Float _maxT;

    public:
        Ray() : _origin(0), _dir(0), _isPrimary(false) { }
        Ray(const Point3& origin, const Vec3& direction) 
            : _origin(origin), _dir(direction), _minT(F_RAY_OFFSET), _maxT(F_INFINITY), _isPrimary(false) { }

        Ray(const Point3& origin, const Vec3& direction, Float minT, Float maxT = F_INFINITY)
            : _origin(origin), _dir(direction), _minT(minT), _maxT(maxT), _isPrimary(false) { }

        Ray(const Point3& origin, const Point3& target)
            : _origin(origin), _minT(F_RAY_OFFSET), _isPrimary(false) {
        
            _dir = normalize(target - origin);
            _maxT = arg(target);
        }

        const Point3& origin() const;
        const Vec3& dir() const;
        Float maxT() const;
        Float minT() const;
        bool isPrimary() const;
        
        Point3 hitPoint() const;
        Point3 Ray::operator()(Float t) const;
        Float arg(const Point3& point) const;
        bool inRange(Float t) const;
        
        void setMaxT(Float t) const;
        void setPrimary(bool isPrimary);

        Ray reflect(const SurfaceEvent& evt) const;
        Ray refract(const SurfaceEvent& evt, Float inIOR) const;
    };

    class RayEvent {
    public:
        RayEvent() 
            : _point(0), _normal(0), _wo(0) { }

        RayEvent(const Ray& ray) 
            : _point(ray.hitPoint()), _wo(-ray.dir()), _normal(0) { }

        RayEvent(const Point3& point, const Normal& normal, const Vec3& wo)
            : _point(point), _normal(normal), _wo(wo) { }

        Vec3 wo() const {
            return _wo;
        }

        Normal normal() const {
            return _normal;
        }

        Point3 point() const {
            return _point;
        }

        Ray spawnRay(const Point3& target) const {
            return Ray(_point, target);
        }

    protected:
        Point3 _point;   // Point of interaction
        Vec3   _wo;      // -ray.dir, the outgoing direction in shading calculations
        Normal _normal;  // Normal at interaction point
    };

    class SurfaceEvent : public RayEvent {
    public:
        SurfaceEvent() 
            : RayEvent(), _obj(nullptr), _backface(false) { }

        SurfaceEvent(const Ray& ray, const Geometry* obj) 
            : RayEvent(ray), _obj(obj), _backface(false) { }

        void setEvent(const Ray& ray, const Geometry* obj, const Normal& normal) {
            _obj = obj;
            _point = ray.hitPoint();
            _normal = normal;
            _wo = -ray.dir();
        }

        void setObj(const Geometry* obj) {
            _obj = obj;
        }

        void setBackface(bool backface) {
            _backface = backface;
        }

        const Geometry* obj() const {
            return _obj;
        }

        bool hit() const {
            return _obj != nullptr;
        }

        bool backface() const {
            return _backface;
        }

    private:  
        const Geometry* _obj;
        bool _backface;
    };

}