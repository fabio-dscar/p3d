#pragma once

#include <Vector.h>
#include <Shape.h>

namespace Photon {

    class Cylinder : public Shape {
    public:
        Cylinder(const Point3& base, Float baseRadius, const Point3& apex, Float apexRadius) :
            _base(base), _baseRadius(baseRadius), _apex(apex), _apexRadius(apexRadius) { }

    private:
        Point3 _base;
        Float  _baseRadius;
        Point3 _apex;
        Float  _apexRadius;
    };

}