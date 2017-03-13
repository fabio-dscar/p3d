#pragma once

#include <Vector.h>

namespace Photon {

    // Forward declarations
    class Geometry;

    struct HitInfo;

    class Ray {
    public:
        Ray() : _origin(0.0f), _dir(0.0f) { }
        Ray(const Vec3& origin, const Vec3& direction) 
            : _origin(origin), _dir(direction) { }

        const Vec3& getOrigin() const;
        const Vec3& getDirection() const;
        const Vec3 getPoint(float arg) const;
        float getArg(const Vec3& point) const;

        Ray reflect(const HitInfo& info) const;
        Ray refract(const HitInfo& info, float inIOR) const;

    private:
        Vec3 _origin;
        Vec3 _dir;
    };

    struct HitInfo {
        Vec3 _point;
        Vec3 _normal;
        float _t;
        bool _hit;
        Geometry* _obj;
        Ray _ray;
    };
}