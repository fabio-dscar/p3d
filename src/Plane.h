#pragma once

#include <Vector.h>
#include <Geometry.h>
#include <Ray.h>

namespace Photon {

    class Plane : public Geometry {
    public:
        Plane(const Vec3& normal, float distance) : 
            _normal(normal), _distance(distance) { }

        const Vec3& getNormal() const;
        float getDistance() const;

        bool intersectRay(const Ray& ray, HitInfo& info) const;

    private:
        Vec3 _normal;
        float _distance; // Distance to origin
    };

}