#pragma once

#include <vector>

#include <Vector.h>
#include <Geometry.h>

namespace Photon {

    class Polygon : public Geometry {
    public:
        Polygon() { }

        void addVertex(const Point3& vertex) {
            _vertices.push_back(vertex);
        }

        const std::vector<Point3>& getVertices() const {
            return _vertices;
        }

        void setNormal(const Normal& normal) {
            _normal = normal;
        }

        const Normal& getNormal() const {
            return _normal;
        }

    private:
        std::vector<Point3> _vertices;
        Normal _normal;
    };

}