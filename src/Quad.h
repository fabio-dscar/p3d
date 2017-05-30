#pragma once

#include <Shape.h>

namespace Photon {

    class Quad : public Shape {
    public:
        Quad(const Transform& objToWorld);

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;

        void computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const;

        Bounds3 bbox() const;
        Float area() const;

        void samplePosition(const Point2& rand, PositionSample* sample) const;
        Float pdfPosition(const PositionSample& sample) const;

        void sampleDirect(const Point2& rand, DirectSample* sample) const;
        Float pdfDirect(const DirectSample& sample) const;

    private:
        Float _area;
    };

}