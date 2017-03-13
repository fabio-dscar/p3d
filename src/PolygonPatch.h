#pragma once

#include <vector>

#include <Vector.h>
#include <Geometry.h>

namespace Photon {

    struct PatchVertex {
        Vec3 _vertex;
        Vec3 _normal;
    };

    class PolygonPatch : public Geometry {
    public:
        PolygonPatch() {}

        void addVertex(const Vec3& vertex, const Vec3& normal) {
            _vertices.push_back({ vertex, normal });
        }

        const std::vector<PatchVertex>& getVertices() const {
            return _vertices;
        }

        void setNormal(const Vec3& normal) {
            _normal = normal;
        }

        const Vec3& getNormal() const {
            return _normal;
        }

    private:
        std::vector<PatchVertex> _vertices;
        Vec3 _normal;
    };

}