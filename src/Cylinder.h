#pragma once

#include <Vector.h>
#include <Geometry.h>

namespace Photon {

    class Cylinder : public Geometry {
    public:
        Cylinder(const Vec3& base, float baseRadius, const Vec3& apex, float apexRadius) :
            _base(base), _baseRadius(baseRadius), _apex(apex), _apexRadius(apexRadius) { }

    private:
        Vec3 _base;
        float _baseRadius;
        Vec3 _apex;
        float _apexRadius;
    };

}