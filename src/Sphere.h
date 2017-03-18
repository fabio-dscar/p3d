#pragma once

#include <Vector.h>
#include <Geometry.h>

#include <glm/gtx/norm.hpp>

namespace Photon {

    class Sphere : public Geometry {
    public:
        Sphere(const Vec3& position, float radius) 
            : _pos(position), _radius(radius) { }
        
        const Vec3& pos() const;
        float radius() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;
    private:
        Vec3 _pos;
        float _radius;
    };

}