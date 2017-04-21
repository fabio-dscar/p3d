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
        void  setMaxT(Float t) const;
        bool  inRange(Float t) const;

        bool  isPrimary() const;
        void  setPrimary(bool isPrimary);
        
        Float arg(const Point3& point) const;
        Point3 Ray::operator()(Float t) const;

        Ray reflect(const SurfaceEvent& evt) const;
        Ray refract(const SurfaceEvent& evt, Float inIOR) const;
    };


    class RayEvent {
    public:
        /*Point3 point;
        Normal normal;
        Vec3   wo;
        */

        RayEvent() : _point(0), _normal(0), _wo(0) { }
        RayEvent(const Ray& ray);
        RayEvent(const Point3& point, const Normal& normal);
        RayEvent(const Point3& point, const Normal& normal, const Vec3& wo);

        Vec3   wo()     const;
        Normal normal() const;
        Point3 point()  const;

        Ray spawnRay(const Point3& target) const;
        Ray spawnRay(const Vec3& dir) const;
        Ray spawnRay(const Vec3& dir, Float dist) const;

    protected:
        Point3 _point;   // Point of interaction
        Vec3   _wo;      // -ray.dir, the outgoing direction in shading calculations
        Normal _normal;  // Normal at interaction point (geometric)
    };


    class SurfaceEvent : public RayEvent {
    public:
        /*const Shape* obj;
        Point2 uv;
        Frame gFrame;
        Frame sFrame;
        bool  backface;*/

        SurfaceEvent() 
            : RayEvent(), _obj(nullptr), _backface(false) { }

        SurfaceEvent(const Ray& ray, Shape const* obj) 
            : RayEvent(ray), _obj(obj), _backface(false) { }

        Shape const* obj() const;
        const Point2& uv() const;
        bool backface() const;

        void setEvent(const Ray& ray, Shape const* obj, const Normal& normal);
        void setPoint(const Point3& pt);
        void setShadFrame(const Frame& shframe);
        void setGeoFrame(const Frame& gframe);
        void setObj(const Shape* obj);
        void setUV(const Point2& uv);
        void setBackface(bool backface);        
        bool hit() const;       
        Color Le(const Vec3& w) const;

        const Frame& gFrame() const;
        const Frame& sFrame() const;

        Vec3 toWorld(const Vec3& w) const;
        Vec3 toLocal(const Vec3& w) const;

    private:  
        const Shape* _obj;
        Point2 _uv;
        Frame  _gFrame;
        Frame  _sFrame;
        bool   _backface;
    };

}