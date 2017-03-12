#pragma once

#include <Vector.h>

#include <Ray.h>

namespace Photon {

    class Plane {
    public:
        Plane(const Vec3& normal, float distance) : 
            _normal(normal), _distance(distance) { }

        bool intersectRay(const Ray& ray) {
            float NdotD = glm::dot(_normal, ray.getDirection());
            if (NdotD > 1e-6) {
                float NdotO = glm::dot(_normal, ray.getOrigin());
                float t = (_distance - NdotO) / NdotD;

                return true;
            }

            return false;
        }

    private:
        Vec3 _normal;
        float _distance;
    };

}