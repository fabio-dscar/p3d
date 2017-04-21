#pragma once

#include <Light.h>

namespace Photon {

    class Shape;

    class AreaLight : public Light {
    public:
        AreaLight() : Light(), _shape(nullptr) { }
        AreaLight(std::shared_ptr<Shape> shape, const Color& emission, uint32 nSamples) 
            : Light(emission, nSamples), _shape(shape) { }

        Color L(const RayEvent& evt, const Vec3& w) const;
        Float area() const;

        bool isDelta() const {
            return false;
        }

        void sampleLi(const RayEvent& ref, const Point2& rand, Point3* pos, Float* pdf) const;






        Color evalL(const SurfaceEvent& it, const Vec3& wo) const;

        Color samplePosition(const Point2& rand, PositionSample* sample) const;
        Float pdfPosition(const PositionSample& sample) const;

        Color sampleDirect(const Point2& rand, DirectSample* sample) const;
        Float pdfDirect(const DirectSample& sample) const;

        Color sampleEmitDirection(const Point2& rand, const PositionSample& pos, DirectionSample* sample) const;
        Float pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const;

    private:
        //Color _Le;
        std::shared_ptr<Shape> _shape;
    };

}