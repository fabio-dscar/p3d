#pragma once

#include <Vector.h>
#include <Geometry.h>
#include <Bounds.h>

namespace Photon {

    class Sphere : public Geometry {
    public:
        Sphere(const Point3& position, Float radius)
            : _pos(position), _radius(radius) { }
        
        const Point3& pos() const;
        Float radius() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;

        Bounds3 bounds() const {
            Vec3 radVec(_radius + F_EPSILON);

            return Bounds3(_pos - radVec, _pos + radVec);
        }
    private:
        Point3 _pos;
        Float _radius;
    };

}