#pragma once

#include <array>

#include <Vector.h>
#include <Shape.h>

namespace Photon {

    class Triangle : public Shape {
    public:
        Triangle(const Point3& p1, const Point3& p2, const Point3& p3);
        Triangle(const Point3& p1, const Point3& p2, const Point3& p3, const Normal& normal)
            : _vertices{ p1, p2, p3 }, _normal(normal) { }

        void setVertices(const Point3& p1, const Point3& p2, const Point3& p3);
        const std::array<Point3, 3>& vertices() const;

        const Normal& normal() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* info) const;
        bool isOccluded(const Ray& ray) const;

        Bounds3 bbox() const;
    private:
        std::array<Point3, 3> _vertices;
        Normal _normal;
    };

}