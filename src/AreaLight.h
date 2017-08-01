#pragma once

#include <Light.h>

namespace Photon {

    class Shape;

    class AreaLight : public Light {
    public:
        AreaLight() : Light(), _shape(nullptr), _numSamples(0) { }
        AreaLight(std::shared_ptr<Shape> shape, const Color& emission, uint32 nSamples) 
            : Light(emission), _shape(shape), _numSamples(nSamples) { }

        std::shared_ptr<Shape> shape() const;

        Color L(const RayEvent& evt, const Vec3& w) const;
        Float area() const;
            
        bool isDelta() const;
        uint32 numSamples() const;
        Color power() const;

        Color evalL(const SurfaceEvent& it, const Vec3& wo) const;
        Color evalL(const PositionSample& sample, const Vec3& wo) const;

        Color samplePosition(const Point2& rand, PositionSample* sample) const;
        Float pdfPosition(const PositionSample& sample) const;

        Color sampleDirect(const Point2& rand, DirectSample* sample) const;
        Float pdfDirect(const DirectSample& sample) const;

        Color sampleEmitDirection(const Point2& rand, const PositionSample& pos, DirectionSample* sample) const;
        Float pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const;

    private:
        uint32 _numSamples;

        std::shared_ptr<Shape> _shape;
    };

}