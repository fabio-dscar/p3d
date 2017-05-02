#pragma once

#include <Vector.h>

namespace Photon {

    struct Vertex {
        Point3 vertex;
        Normal normal;
        Vec3   tangent;
        Vec2   uv;

        Vertex() : vertex(0), normal(0), uv(0), tangent(0) { }

        Vertex(const Point3& vert) 
            : vertex(vert), normal(0), uv(0), tangent(0) { }

        Vertex(const Point3& vert, const Normal& norm) 
            : vertex(vert), normal(norm), uv(0), tangent(0) { }

        Vertex(const Point3& vert, const Normal& norm, const Vec2& uv) 
            : vertex(vert), normal(norm), uv(uv), tangent(0) { }
    };

}