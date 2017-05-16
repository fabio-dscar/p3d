#pragma once

#include <PhotonMath.h>
#include <Vector.h>
#include <Spectral.h>
#include <Frame.h>

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
        Float         _time;

    public:
        Ray() : _origin(0), _dir(0), _isPrimary(false), _time(0) { }
        Ray(const Point3& origin, const Vec3& dir);
        Ray(const Point3& origin, const Vec3& dir, Float minT, Float maxT = F_INFINITY);
        Ray(const Point3& origin, const Point3& target);

        const Point3& origin() const;
        const Vec3&   dir() const;
        Float time() const;

        Point3 hitPoint() const;

        Float minT() const;
        Float maxT() const;
        void  setMaxT(Float t) const;
        bool  inRange(Float t) const;

        bool  isPrimary() const;
        void  setPrimary(bool isPrimary);
        
        void setTime(Float time);

        Float arg(const Point3& point) const;
        Point3 Ray::operator()(Float t) const;
    };


    class RayEvent {
    public:
        Point3 point;
        Normal normal;
        Vec3   wo;
        
        RayEvent() : point(0), normal(0), wo(0) { }
        RayEvent(const Ray& ray);
        RayEvent(const Point3& point, const Normal& normal);
        RayEvent(const Point3& point, const Normal& normal, const Vec3& wo);

        Ray spawnRay(const Point3& target) const;
        Ray spawnRay(const Vec3& dir) const;
        Ray spawnRay(const Vec3& dir, Float dist) const;
    };


    class SurfaceEvent : public RayEvent {
    public:
        const Shape* obj;
        Point2 uv;
        Frame  gFrame;
        Frame  sFrame;
        bool   backface;

        SurfaceEvent() 
            : RayEvent(), obj(nullptr), backface(false) { }

        SurfaceEvent(const Ray& ray, Shape const* obj) 
            : RayEvent(ray), obj(obj), backface(false) { }

        bool hit() const;
        void setEvent(const Ray& ray, Shape const* obj, const Normal& normal);                 
        Color emission(const Vec3& w) const;

        Vec3 toWorld(const Vec3& w) const;
        Vec3 toLocal(const Vec3& w) const;
    };

}