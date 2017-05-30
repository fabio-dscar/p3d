#pragma once

#include <Vector.h>
#include <Shape.h>

namespace Photon {

    class Box : public Shape {
    public:
        Box(const Point3& min, const Point3& max);
        Box(const Transform& objToWorld);

        Bounds3 bbox() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;

    private:
        Point3 center() const;
        Normal calcNormal(const Point3& pt) const;

        Point3 _min;
        Point3 _max;
    };

}