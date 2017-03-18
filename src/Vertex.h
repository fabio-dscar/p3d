#pragma once

#include <Vector.h>

namespace Photon {

    struct Vertex {
        Vec3 _vertex;
        Vec3 _normal;
        Vec2 _uv;

        Vertex() : _vertex(0.0f), _normal(0.0f), _uv(0.0f) { }
        Vertex(const Vec3& vertex) : _vertex(vertex), _normal(0.0f), _uv(0.0f) { }
        Vertex(const Vec3& vertex, const Vec3& normal) : _vertex(vertex), _normal(normal), _uv(0.0f) { }
        Vertex(const Vec3& vertex, const Vec3& normal, const Vec2& uv) : _vertex(vertex), _normal(normal), _uv(uv) { }
    };

}