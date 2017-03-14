#pragma once

#include <Vector.h>
#include <MathDefs.h>

namespace Photon {

    // Forward declarations
    class Geometry;

    struct HitInfo;

    class Ray {
    public:
        Ray() : _origin(0.0f), _dir(0.0f) { }
        Ray(const Vec3& origin, const Vec3& direction) 
            : _origin(origin), _dir(direction), _nearT(F_RAY_OFFSET), _farT(F_INFINITY) { }

        Ray(const Vec3& origin, const Vec3& direction, float nearT, float farT = F_INFINITY)
            : _origin(origin), _dir(direction), _nearT(nearT), _farT(farT) { }

        Ray(const Vec3& origin, const Vec3& direction, const Vec3& target)
            : _origin(origin), _dir(direction), _nearT(F_RAY_OFFSET) {
        
            _farT = getTime(target);
        }

        const Vec3& getOrigin() const;
        const Vec3& getDirection() const;
        const Vec3 getPoint(float arg) const;
        
        float getFarTime() const;
        float getNearTime() const;
        bool isValidTime(float t) const;
        float getTime(const Vec3& point) const;

        Ray reflect(const HitInfo& info) const;
        Ray refract(const HitInfo& info, float inIOR) const;

    private:
        Vec3 _origin;
        Vec3 _dir;
        float _nearT;
        float _farT;
    };

    struct HitInfo {
        Vec3 _point;
        Vec3 _normal;
        float _t;
        bool _hit;
        Geometry* _obj;
        Ray _ray;
        bool _backface;

        HitInfo() {
            _backface = false;
            _hit = false;
            _obj = nullptr;
        }
    };
}