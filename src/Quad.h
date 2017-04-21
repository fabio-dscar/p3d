#pragma once

#include <Shape.h>

namespace Photon {

    class Quad : public Shape {
    public:
        Quad() { }

        virtual bool intersectRay(const Ray& ray, SurfaceEvent* evt) const {
            Normal normal = Normal((cross(_edgeOne, _edgeTwo)));
            normal.normalize();

            Float NdotD = dot(normal, ray.dir());
            if (std::abs(NdotD) > F_EPSILON) {
                Float NdotO = dot(normal, ray.origin().posVec());
                Float t = (dist(_pos, Point3(0)) - NdotO) / NdotD;

                if (ray.inRange(t)) {
                    ray.setMaxT(t);
                    evt->setEvent(ray, this, normal);
                    return true;
                }
            }
        }

        virtual bool isOccluded(const Ray& ray) const {
            return false;
        }

        virtual Bounds3 bbox() const {
            return Bounds3(Point3(0));
        }

    private:
        Point3 _pos;
        Vec3   _edgeOne;
        Vec3   _edgeTwo;
    };

}