#pragma once

#include <Vector.h>
#include <Geometry.h>
#include <Ray.h>

namespace Photon {

    class Plane : public Geometry {
    public:
        Plane(const Vec3& normal, float distance) : 
            _normal(normal), _distance(distance) { }

        bool intersectRay(const Ray& ray, HitInfo& info) const {
            float NdotD = glm::dot(_normal, ray.getDirection());
            if (std::abs(NdotD) > 1e-6) {
                float NdotO = glm::dot(_normal, ray.getOrigin());

                info._t = (_distance - NdotO) / NdotD;
                info._point = ray.getPoint(info._t);
                info._normal = _normal;
                info._obj = (Geometry*)this;
                info._hit = true;

                return true;
            }

            return false;
        }

    private:
        Vec3 _normal;
        float _distance;
    };

}