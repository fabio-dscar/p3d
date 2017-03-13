#pragma once

#include <Vector.h>

namespace Photon {

    // Forward declaration
    class Geometry;

    class Ray {
    public:
        Ray() : _origin(0.0f), _dir(0.0f) { }
        Ray(const Vec3& origin, const Vec3& direction) : _origin(origin), _dir(direction) { }

        const Vec3& getOrigin() const {
            return _origin;
        }

        const Vec3& getDirection() const {
            return _dir;
        }

        const Vec3 getPoint(float arg) const {
            return _origin + arg * _dir;
        }

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