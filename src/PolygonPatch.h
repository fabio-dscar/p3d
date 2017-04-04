#pragma once

#include <vector>

#include <Vertex.h>
#include <Shape.h>

namespace Photon {

    class PolygonPatch : public Shape {
    public:
        PolygonPatch() {}

        void addVertex(const Point3& vertex, const Normal& normal) {
            _vertices.push_back({ vertex, normal });
        }

        void addVertex(const Point3& vertex, const Normal& normal, const Vec2& uv) {
            _vertices.push_back({ vertex, normal, uv });
        }

        const std::vector<Vertex>& getVertices() const {
            return _vertices;
        }

        void setNormal(const Normal& normal) {
            _normal = normal;
        }

        const Normal& getNormal() const {
            return _normal;
        }

    private:
        std::vector<Vertex> _vertices;
        Normal _normal;
    };

}