#pragma once

#include <Vector.h>
#include <Shape.h>
#include <Ray.h>

namespace Photon {

    class Plane : public Shape {
    public:
        Plane(const Normal& normal, Float distance) :
            _normal(normal), _distance(distance) { }

        const Normal& normal() const;
        Float distance() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;

        Bounds3 bbox() const;
    private:
        Normal _normal;
        Float  _distance; // Distance to origin
    };

}