#pragma once

#include <array>

#include <Vector.h>
#include <Geometry.h>

namespace Photon {

    class Triangle : public Geometry {
    public:
        Triangle(const Vec3& v1, const Vec3& v2, const Vec3& v3);
        Triangle(const Vec3& v1, const Vec3& v2, const Vec3& v3, const Vec3& normal) 
            : _vertices{ v1, v2, v3 }, _normal(normal) { }

        void setVertices(const Vec3& v1, const Vec3& v2, const Vec3& v3);
        const std::array<Vec3, 3>& vertices() const;

        const Vec3& normal() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* info) const;
        bool isOccluded(const Ray& ray) const;
    private:
        std::array<Vec3, 3> _vertices;
        Vec3 _normal;
    };

}