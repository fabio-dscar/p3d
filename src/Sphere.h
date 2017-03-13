#pragma once

#include <Vector.h>
#include <Geometry.h>

#include <glm/gtx/norm.hpp>

namespace Photon {

    class Sphere : public Geometry {
    public:
        Sphere(const Vec3& position, float radius) : _pos(position), _radius(radius) { }
        
        bool intersectRay(const Ray& ray, HitInfo& info) const {
            /*float t0, t1;
            float radiusSqr = _radius * _radius;

            // Geometric solution
            Vec3 l = _pos - ray.getOrigin();
            float tca = glm::dot(l, ray.getDirection());
            if (tca < 0) 
                return false;

            float d2 = glm::dot(l, l) - tca * tca;
            if (d2 > radiusSqr)
                return false;

            float thc = sqrt(radiusSqr - d2);
            t0 = tca - thc;
            t1 = tca + thc;

            if (t0 > t1) std::swap(t0, t1);

            if (t0 < 0.0f) {
                t0 = t1; // if t0 is negative, let's use t1 instead 
                if (t0 < 0.0f) 
                    return false; // both t0 and t1 are negative 
            }

            info._t = t0;
            info._point = ray.getPoint(info._t);
            info._obj = (Geometry*)this;
            info._normal = glm::normalize(info._point - _pos);
            info._hit = true;

            return true;*/
            
            float dSqr = glm::length2(ray.getOrigin() - _pos);
            float radiusSqr = _radius * _radius;
            if (dSqr == radiusSqr)
                return false;
            
            float b = glm::dot(ray.getDirection(), _pos - ray.getOrigin());

            if (dSqr > radiusSqr) {
                if (b < 0)
                    return false;
            }

            float r = b * b - dSqr + radiusSqr;
            if (r < 0)
                return false;

            float t;
            if (dSqr > radiusSqr)
                t = b - std::sqrt(r);
            else if (dSqr < radiusSqr)
                t = b + std::sqrt(r);

            info._hit = true;
            info._t = t;
            info._point = ray.getPoint(t);
            info._normal = glm::normalize((info._point - _pos) / _radius);
            if (dSqr < radiusSqr)
                info._normal = -info._normal;
            info._obj = (Geometry*)this;

            return true;
        }

    private:
        Vec3 _pos;
        float _radius;
    };

}