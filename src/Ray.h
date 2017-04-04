#pragma once

#include <Vector.h>
#include <PhotonMath.h>

namespace Photon {

    // Forward declarations
    class Geometry;
    class SurfaceEvent;
    class Shape;

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
        Ray(const Point3& origin, const Vec3& dir);
        Ray(const Point3& origin, const Vec3& dir, Float minT, Float maxT = F_INFINITY);
        Ray(const Point3& origin, const Point3& target);

        const Point3& origin() const;
        const Vec3&   dir() const;

        Point3 hitPoint() const;

        Float minT() const;
        Float maxT() const;
        void setMaxT(Float t) const;
        bool inRange(Float t) const;

        bool isPrimary() const;
        void setPrimary(bool isPrimary);
        
        Float arg(const Point3& point) const;
        Point3 Ray::operator()(Float t) const;

        Ray reflect(const SurfaceEvent& evt) const;
        Ray refract(const SurfaceEvent& evt, Float inIOR) const;
    };


    class RayEvent {
    public:
        RayEvent() : _point(0), _normal(0), _wo(0) { }
        RayEvent(const Ray& ray);
        RayEvent(const Point3& point, const Normal& normal);
        RayEvent(const Point3& point, const Normal& normal, const Vec3& wo);

        Vec3 wo() const;
        Normal normal() const;
        Point3 point() const;

        Ray spawnRay(const Point3& target) const;

    protected:
        Point3 _point;   // Point of interaction
        Vec3   _wo;      // -ray.dir, the outgoing direction in shading calculations
        Normal _normal;  // Normal at interaction point
    };


    class SurfaceEvent : public RayEvent {
    public:
        SurfaceEvent() 
            : RayEvent(), _obj(nullptr), _backface(false) { }

        SurfaceEvent(const Ray& ray, Shape const* obj) 
            : RayEvent(ray), _obj(obj), _backface(false) { }

        void setEvent(const Ray& ray, Shape const* obj, const Normal& normal);
        void setObj(const Shape* obj);
        void setBackface(bool backface);
        Shape const* obj() const;
        bool hit() const;
        bool backface() const;
        Color3 Le(const Vec3& w) const;

    private:  
        const Shape* _obj;
        bool _backface;
    };

}