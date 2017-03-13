#pragma once

#include <Vector.h>
#include <Geometry.h>

#include <glm/gtx/norm.hpp>

namespace Photon {

    class Sphere : public Geometry {
    public:
        Sphere(const Vec3& position, float radius) 
            : _pos(position), _radius(radius) { }
        
        const Vec3& getPosition() const;
        float getRadius() const;

        bool intersectRay(const Ray& ray, HitInfo& info) const;

    private:
        Vec3 _pos;
        float _radius;
    };

}