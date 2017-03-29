#pragma once

#include <Vector.h>

namespace Photon {

    struct Vertex {
        Point3 _vertex;
        Normal _normal;
        Vec2   _uv;

        Vertex() : _vertex(0.0f), _normal(0.0f), _uv(0.0f) { }
        Vertex(const Point3& vertex) : _vertex(vertex), _normal(0.0f), _uv(0.0f) { }
        Vertex(const Point3& vertex, const Normal& normal) : _vertex(vertex), _normal(normal), _uv(0.0f) { }
        Vertex(const Point3& vertex, const Normal& normal, const Vec2& uv) : _vertex(vertex), _normal(normal), _uv(uv) { }
    };

}