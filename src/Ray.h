#pragma once

#include <Vector.h>

namespace Photon {

    class Ray {
    public:
        Ray(const Vec3& origin, const Vec3& direction) : _origin(origin), _dir(direction) { }

        const Vec3& getOrigin() const {
            return _origin;
        }

        const Vec3& getDirection() const {
            return _dir;
        }

    private:
        Vec3 _origin;
        Vec3 _dir;
    };

}