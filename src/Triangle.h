#pragma once

#include <array>

#include <Vector.h>
#include <Geometry.h>

namespace Photon {

    class Triangle : public Geometry {
    public:
        Triangle(const Point3& p1, const Point3& p2, const Point3& p3);
        Triangle(const Point3& p1, const Point3& p2, const Point3& p3, const Normal& normal)
            : _vertices{ p1, p2, p3 }, _normal(normal) { }

        void setVertices(const Point3& p1, const Point3& p2, const Point3& p3);
        const std::array<Point3, 3>& vertices() const;

        const Normal& normal() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* info) const;
        bool isOccluded(const Ray& ray) const;

        Bounds3 bounds() const {
            Point3 max = Math::max(_vertices[0], Math::max(_vertices[1], _vertices[2]));
            Point3 min = Math::min(_vertices[0], Math::min(_vertices[1], _vertices[2]));

            return Bounds3(min + -F_EPSILON, max + F_EPSILON);
        }
    private:
        std::array<Point3, 3> _vertices;
        Normal _normal;
    };

}