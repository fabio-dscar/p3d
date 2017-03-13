#pragma once

#include <vector>

#include <Vector.h>
#include <Geometry.h>

namespace Photon {

    class Polygon : public Geometry {
    public:
        Polygon() { }

        void addVertex(const Vec3& vertex) {
            _vertices.push_back(vertex);
        }

        const std::vector<Vec3>& getVertices() const {
            return _vertices;
        }

        void setNormal(const Vec3& normal) {
            _normal = normal;
        }

        const Vec3& getNormal() const {
            return _normal;
        }

    private:
        std::vector<Vec3> _vertices;
        Vec3 _normal;
    };

}