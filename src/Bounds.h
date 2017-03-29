#pragma once

#include <Vector.h>

namespace Photon {

    class Ray;

    class Bounds3 {
    public:
        Bounds3() : _min(-F_INFINITY), _max(F_INFINITY) { }
        Bounds3(const Point3& min, const Point3& max)
            : _min(min), _max(max) { }

        const Point3& min() const;
        const Point3& max() const;
        bool contains(const Point3& pos) const;
        bool intersectPts(const Ray& ray, Float* t0, Float* t1) const;

        const Point3& operator[](uint32 i) const;
        bool overlaps(const Bounds3& box) const;

        bool bounded() const;
    private:
        Point3 _min;
        Point3 _max;
    };

    
    inline Bounds3 expand(const Bounds3& box, const Point3& pt) {
        return Bounds3(
            Point3(std::min(box[0].x, pt.x),
                   std::min(box[0].y, pt.y),
                   std::min(box[0].z, pt.z)),
            Point3(std::max(box[1].x, pt.x),
                   std::max(box[1].y, pt.y),
                   std::max(box[1].z, pt.z)));
    }

    inline Bounds3 expand(const Bounds3& box1, const Bounds3& box2) {
        return Bounds3(Point3(std::min(box1[0].x, box2[0].x),
                              std::min(box1[0].y, box2[0].y),
                              std::min(box1[0].z, box2[0].z)),
                       Point3(std::max(box1[1].x, box2[1].x),
                              std::max(box1[1].y, box2[1].y),
                              std::max(box1[1].z, box2[1].z)));
    }

    inline Bounds3 intersection(const Bounds3& box1, const Bounds3& box2) {
        return Bounds3(Point3(std::max(box1[0].x, box2[0].x),
                              std::max(box1[0].y, box2[0].y),
                              std::max(box1[0].z, box2[0].z)),
                       Point3(std::min(box1[1].x, box2[1].x),
                              std::min(box1[1].y, box2[1].y),
                              std::min(box1[1].z, box2[1].z)));
    }

    inline bool overlaps(const Bounds3& box1, const Bounds3& box2) {
        return (box1[1].x >= box2[0].x) && (box1[0].x <= box2[1].x) &&
               (box1[1].y >= box2[0].y) && (box1[0].y <= box2[1].y) && 
               (box1[1].z >= box2[0].z) && (box1[0].z <= box2[1].z);
    }

}