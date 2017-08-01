#pragma once

#include <Vector.h>
#include <Shape.h>
#include <Bounds.h>
#include <Sampling.h>
#include <Frame.h>

namespace Photon {

    class Sphere : public Shape {
    public:
        Sphere(const Point3& position, Float radius)
            : _pos(position), _radius(radius) { }
        
        const Point3& pos() const;
        Float radius() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;

        void computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const;

        Bounds3 bbox() const;
        Float area() const;

        void  samplePosition(const Point2& rand, PositionSample* sample) const;
        Float pdfPosition(const PositionSample& sample) const;

        void  sampleDirect(const Point2& rand, DirectSample* sample) const;
        Float pdfDirect(const DirectSample& sample) const;

    private:
        Point3 _pos;
        Float  _radius;
    };

}