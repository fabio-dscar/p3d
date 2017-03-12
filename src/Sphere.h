#pragma once

#include <Vector.h>

namespace Photon {

    class Sphere {
    public:
        Sphere(const Vec3& position, float radius) : _pos(position), _radius(radius) { }

    private:
        Vec3 _pos;
        float _radius;
    };

}