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
        Vec3 _origin;
        Vec3 _dir;
        float _minT;
        mutable float _maxT;
        bool _isPrimary;

    public:
        Ray() : _origin(0.0f), _dir(0.0f), _isPrimary(false) { }
        Ray(const Vec3& origin, const Vec3& direction) 
            : _origin(origin), _dir(direction), _minT(F_RAY_OFFSET), _maxT(F_INFINITY), _isPrimary(false) { }

        Ray(const Vec3& origin, const Vec3& direction, float minT, float maxT = F_INFINITY)
            : _origin(origin), _dir(direction), _minT(minT), _maxT(maxT), _isPrimary(false) { }

        Ray(const Vec3& origin, const Vec3& direction, const Vec3& target)
            : _origin(origin), _dir(direction), _minT(F_RAY_OFFSET), _isPrimary(false) {
        
            _maxT = arg(target);
        }

        const Vec3& origin() const;
        const Vec3& dir() const;
        float farT() const;
        float nearT() const;
        bool isPrimary() const;
        
        Vec3 hitPoint() const;
        Vec3 Ray::operator()(float t) const;
        float arg(const Vec3& point) const;
        bool inRange(float t) const;
        
        void setMaxT(float t) const;
        void setPrimary(bool isPrimary);

        Ray reflect(const SurfaceEvent& evt) const;
        Ray refract(const SurfaceEvent& evt, float inIOR) const; 
    };

    class RayEvent {
    public:
        RayEvent() 
            : _point(0), _normal(0), _wo(0) { }

        RayEvent(const Ray& ray) 
            : _point(ray.hitPoint()), _wo(-ray.dir()), _normal(0) { }

        RayEvent(const Vec3& point, const Vec3& normal, const Vec3& wo) 
            : _point(point), _normal(normal), _wo(wo) { }

        Vec3 wo() const {
            return _wo;
        }

        Vec3 normal() const {
            return _normal;
        }

        Vec3 point() const {
            return _point;
        }

    protected:
        Vec3 _point;   // Point of interaction
        Vec3 _wo;      // -ray.dir, the outgoing direction in shading calculations
        Vec3 _normal;  // Normal at interaction point
    };

    class SurfaceEvent : public RayEvent {
    public:
        SurfaceEvent() 
            : RayEvent(), _obj(nullptr), _backface(false) { }

        SurfaceEvent(const Ray& ray, const Geometry* obj) 
            : RayEvent(ray), _obj(obj), _backface(false) { }

        void setEvent(const Ray& ray, const Geometry* obj, const Vec3& normal) {
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