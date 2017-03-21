#pragma once

#include <Vector.h>
#include <Geometry.h>

namespace Photon {

    class Box : public Geometry {
    public:
        Box(const Vec3& min, const Vec3& max) : _min(min), _max(max) { }

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const {
            float tmin = -F_INFINITY, tmax = F_INFINITY;
    
            const Vec3& rayDir = ray.dir();
            const Vec3& rayOrigin = ray.origin();

            if (rayDir.x != 0.0) {
                float tx1 = (_min.x - rayOrigin.x) / rayDir.x;
                float tx2 = (_max.x - rayOrigin.x) / rayDir.x;

                tmin = std::max(tmin, std::min(tx1, tx2));
                tmax = std::min(tmax, std::max(tx1, tx2));

                if (tmin > tmax)
                    return false;
                if (tmax < 0)
                    return false;
            }

            if (rayDir.y != 0.0) {
                float ty1 = (_min.y - rayOrigin.y) / rayDir.y;
                float ty2 = (_max.y - rayOrigin.y) / rayDir.y;

                tmin = std::max(tmin, std::min(ty1, ty2));
                tmax = std::min(tmax, std::max(ty1, ty2));
            }

            if (rayDir.z != 0.0) {
                float ty1 = (_min.z - rayOrigin.z) / rayDir.z;
                float ty2 = (_max.z - rayOrigin.z) / rayDir.z;

                tmin = std::max(tmin, std::min(ty1, ty2));
                tmax = std::min(tmax, std::max(ty1, ty2));
            }

            if (tmax >= tmin) {
                if (tmin > ray.nearT() && tmin < ray.farT()) {
                    ray.setMaxT(tmin);
                    Vec3 normal = calcNormal(ray.hitPoint());
                    evt->setBackface(false);
                    evt->setEvent(ray, this, glm::normalize(normal));

                    return true;
                } else if (tmax > ray.nearT() && tmax < ray.farT()) {
                    ray.setMaxT(tmax);
                    Vec3 normal = calcNormal(ray.hitPoint());
                    evt->setBackface(true);
                    evt->setEvent(ray, this, -glm::normalize(normal));

                    return true;
                }
            }
            
            return false;
        }

        Vec3 calcNormal(const Vec3& localPoint) const {
            Vec3 size = Vec3(_max.x - _min.x,
                             _max.y - _min.y,
                             _max.z - _min.z);

            Vec3 normal = Vec3(0);
            float min = F_INFINITY;

            float dist = std::abs(size.x - std::abs(localPoint.x));
            if (dist < min) {
                min = dist;
                normal = Vec3(1, 0, 0);
                normal *= glm::sign(localPoint.x);
            }

            dist = std::abs(size.y - std::abs(localPoint.y));
            if (dist < min) {
                min = dist;
                normal = Vec3(0, 1, 0);
                normal *= glm::sign(localPoint.y);
            }

            dist = std::abs(size.z - std::abs(localPoint.z));
            if (dist < min) {
                min = dist;
                normal = Vec3(0, 0, 1);
                normal *= glm::sign(localPoint.z);
            }

            return normal;
        }

        bool isOccluded(const Ray& ray) const {
            float tmin = -F_INFINITY, tmax = F_INFINITY;

            const Vec3& rayDir = ray.dir();
            const Vec3& rayOrigin = ray.origin();

            if (rayDir.x != 0.0) {
                float tx1 = (_min.x - rayOrigin.x) / rayDir.x;
                float tx2 = (_max.x - rayOrigin.x) / rayDir.x;

                tmin = std::max(tmin, std::min(tx1, tx2));
                tmax = std::min(tmax, std::max(tx1, tx2));
            }

            if (rayDir.y != 0.0) {
                float ty1 = (_min.y - rayOrigin.y) / rayDir.y;
                float ty2 = (_max.y - rayOrigin.y) / rayDir.y;

                tmin = std::max(tmin, std::min(ty1, ty2));
                tmax = std::min(tmax, std::max(ty1, ty2));
            }

            if (rayDir.z != 0.0) {
                float tz1 = (_min.z - rayOrigin.z) / rayDir.z;
                float tz2 = (_max.z - rayOrigin.z) / rayDir.z;

                tmin = std::max(tmin, std::min(tz1, tz2));
                tmax = std::min(tmax, std::max(tz1, tz2));
            }

            if (tmax >= tmin) {
                if (tmin > ray.nearT() && tmin < ray.farT())
                    return true;
                else if (tmax > ray.nearT() && tmax < ray.farT()) 
                    return true;
            }

            return false;
        }

    private:
        Vec3 _min;
        Vec3 _max;
    };

}