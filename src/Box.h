#pragma once

#include <Vector.h>
#include <Shape.h>

namespace Photon {

    class Box : public Shape {
    public:
        Box(const Point3& min, const Point3& max) 
            : _min(min), _max(max) { }

        Bounds3 bbox() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;
   

    private:
        Point3 center() const;

        Normal calcNormal(const Point3& pt) const {
            Point3 c = center();

            Vec3 p = pt - c;

            Vec3 sizes = Vec3(_max.x - _min.x, _max.y - _min.y, _max.z - _min.z);
            uint32 dim = (abs(p) - sizes).maxDim();

            Normal n(0);
            n[dim] = sign(p[dim]);

            return normalize(n);

            /*Point3 d = abs(Point3(_max.x - _min.x, _max.y - _min.y, _max.z - _min.z) * 0.5);
            Float bias = 1 + F_EPSILON;

            return Normal(int32((p.x / d.x) * bias), int32((p.y / d.y) * bias), int32((p.z / d.z) * bias));*/
        }

        Point3 _min;
        Point3 _max;
    };

}