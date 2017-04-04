#pragma once

#include <Vector.h>

namespace Photon {

    struct Vertex {
        Point3 _vertex;
        Normal _normal;
        Vec2   _uv;

        Vertex() : _vertex(0), _normal(0), _uv(0) { }

        Vertex(const Point3& vert) 
            : _vertex(vert), _normal(0), _uv(0) { }

        Vertex(const Point3& vert, const Normal& norm) 
            : _vertex(vert), _normal(norm), _uv(0) { }

        Vertex(const Point3& vert, const Normal& norm, const Vec2& uv) 
            : _vertex(vert), _normal(norm), _uv(uv) { }
    };

}