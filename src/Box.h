#pragma once

#include <Vector.h>
#include <Shape.h>

namespace Photon {

    class Box : public Shape {
    public:
        Box(const Point3& min, const Point3& max) 
            : _min(min), _max(max) { }

        Bounds3 bbox() const {
            return Bounds3(_min + Point3(-F_EPSILON), _max + Point3(F_EPSILON));
        }

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const {
            Float tmin = -F_INFINITY, tmax = F_INFINITY;
    
            const Vec3& rayDir = ray.dir();
            const Point3& rayOrigin = ray.origin();

            if (rayDir.x != 0.0) {
                Float tx1 = (_min.x - rayOrigin.x) / rayDir.x;
                Float tx2 = (_max.x - rayOrigin.x) / rayDir.x;

                tmin = std::max(tmin, std::min(tx1, tx2));
                tmax = std::min(tmax, std::max(tx1, tx2));

                if (tmin > tmax)
                    return false;
                if (tmax < 0)
                    return false;
            }

            if (rayDir.y != 0.0) {
                Float ty1 = (_min.y - rayOrigin.y) / rayDir.y;
                Float ty2 = (_max.y - rayOrigin.y) / rayDir.y;

                tmin = std::max(tmin, std::min(ty1, ty2));
                tmax = std::min(tmax, std::max(ty1, ty2));
            }

            if (rayDir.z != 0.0) {
                Float ty1 = (_min.z - rayOrigin.z) / rayDir.z;
                Float ty2 = (_max.z - rayOrigin.z) / rayDir.z;

                tmin = std::max(tmin, std::min(ty1, ty2));
                tmax = std::min(tmax, std::max(ty1, ty2));
            }

            if (tmax >= tmin) {
                if (tmin > ray.minT() && tmin < ray.maxT()) {
                    ray.setMaxT(tmin);
                    Normal normal = calcNormal(ray.hitPoint());
                    evt->setEvent(ray, this, normalize(normal));

                    return true;
                } else if (tmax > ray.minT() && tmax < ray.maxT()) {
                    ray.setMaxT(tmax);
                    Normal normal = calcNormal(ray.hitPoint());
                    evt->setEvent(ray, this, normalize(normal));
                    evt->backface = true;

                    return true;
                }
            }
            
            return false;
        }

        Normal calcNormal(const Point3& localPoint) const {
            Point3 c = Point3(_min.x + _max.x, _min.y + _max.y, _min.z + _max.z) * 0.5;

            Vec3 p = localPoint - c;

            Point3 d = Point3(_max.x - _min.x, _max.y - _min.y, _max.z - _min.z) * 0.5;
            d = abs(d);
            Float bias = 1 + F_EPSILON;

            return Normal(int32((p.x / d.x) * bias), int32((p.y / d.y) * bias), int32((p.z / d.z) * bias));
        }

        bool isOccluded(const Ray& ray) const {
            Float tmin = -F_INFINITY, tmax = F_INFINITY;

            const Vec3& rayDir = ray.dir();
            const Point3& rayOrigin = ray.origin();

            if (rayDir.x != 0.0) {
                Float tx1 = (_min.x - rayOrigin.x) / rayDir.x;
                Float tx2 = (_max.x - rayOrigin.x) / rayDir.x;

                tmin = std::max(tmin, std::min(tx1, tx2));
                tmax = std::min(tmax, std::max(tx1, tx2));
            }

            if (rayDir.y != 0.0) {
                Float ty1 = (_min.y - rayOrigin.y) / rayDir.y;
                Float ty2 = (_max.y - rayOrigin.y) / rayDir.y;

                tmin = std::max(tmin, std::min(ty1, ty2));
                tmax = std::min(tmax, std::max(ty1, ty2));
            }

            if (rayDir.z != 0.0) {
                Float tz1 = (_min.z - rayOrigin.z) / rayDir.z;
                Float tz2 = (_max.z - rayOrigin.z) / rayDir.z;

                tmin = std::max(tmin, std::min(tz1, tz2));
                tmax = std::min(tmax, std::max(tz1, tz2));
            }

            if (tmax >= tmin) {
                if (tmin > ray.minT() && tmin < ray.maxT())
                    return true;
                else if (tmax > ray.minT() && tmax < ray.maxT())
                    return true;
            }

            return false;
        }

    private:
        Point3 _min;
        Point3 _max;
    };

}