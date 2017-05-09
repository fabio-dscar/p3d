#pragma once

#include <Shape.h>

namespace Photon {

    class Quad : public Shape {
    public:
        Quad(const Point3& pos, const Point3& pt1, const Point3& pt2)
            : _pos(pos), _edgeOne(pt1 - pos), _edgeTwo(pt2 - pos) {
            
            Vec3 edgeOne = pt1 - pos;
            Vec3 edgeTwo = pt2 - pos;

            _normal = Normal(normalize(cross(edgeOne, edgeTwo)));
        }

        Quad(const Point3& pos, const Vec3& edge1, const Vec3& edge2)
            : _pos(pos), _edgeOne(edge1), _edgeTwo(edge2) {

            _normal = Normal(normalize(cross(edge1, edge2)));
        }

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const {
            // Check if ray is parallel
            Float NdotD = dot(_normal, ray.dir());
            if (std::abs(NdotD) <= F_EPSILON)
                return false;

            // Compute intersection t
            Float NdotO = dot(_normal, ray.origin().posVec());
            Float dist  = dot(_normal, _pos.posVec());

            Float t = (dist - NdotO) / NdotD;
            if (!ray.inRange(t))
                return false;

            // Check if inside quad bounds
            Point3 hit = ray(t);
            Vec3 uv = hit - _pos;

            // Project onto quad axes
            Vec2 edgeLen = Vec2(_edgeOne.length(), _edgeTwo.length());
            
            Float u = dot(uv, _edgeOne) / edgeLen.x;
            Float v = dot(uv, _edgeTwo) / edgeLen.y;

            if (u < 0 || u > edgeLen.x || v < 0 || v > edgeLen.y)
                return false;

            ray.setMaxT(t);
            evt->setEvent(ray, this, _normal);
            evt->uv.x = u;
            evt->uv.y = v;

            return true;
        }

        bool isOccluded(const Ray& ray) const {
            return false;
        }

        Bounds3 bbox() const {
            return Bounds3(Point3(0));
        }

    private:
        Point3 _pos;
        Vec2   _sizes; // Sizes of the quad in its axes
        Vec3   _edgeOne;
        Vec3   _edgeTwo;
        Normal _normal;
    };

}