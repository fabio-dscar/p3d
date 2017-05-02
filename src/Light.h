#pragma once

#include <Vector.h>
#include <Spectral.h>
#include <Records.h>

namespace Photon {

    class RayEvent;

    class Light {
    public:
        Light() : _Le(1), _numSamples(1) {}
        Light(const Color& emission, uint32 nSamples) : _Le(emission), _numSamples(nSamples) { }
        Light(const Point3& pos) : _Le(1), _numSamples(1) {}
        Light(const Point3& pos, const Color& color) : _Le(color), _numSamples(1) {}

        void setIntensity(const Color& Le);
        const Color& color() const;
        Color L(const RayEvent& evt, const Vec3& w) const;

        uint32 numSamples() const {
            return _numSamples;
        }

        virtual bool isEnvironment() const;
        virtual bool isDelta() const;

        // Evaluate L for outgoing wo at intersection
        virtual Color evalL(const SurfaceEvent& it, const Vec3& wo) const = 0;

        // Sample a position on the surface
        virtual Color samplePosition(const Point2& rand, PositionSample* sample) const = 0;
        virtual Float pdfPosition(const PositionSample& sample) const = 0;

        // Sample an incoming direction to a distant reference position
        virtual Color sampleDirect(const Point2& rand, DirectSample* sample) const = 0;
        virtual Float pdfDirect(const DirectSample& sample) const = 0;

        // Sample an outgoing direction from a local reference position
        virtual Color sampleEmitDirection(const Point2& rand, const PositionSample& pos, DirectionSample* sample) const = 0;
        virtual Float pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const = 0;

    protected:
        Color  _Le;
        uint32 _numSamples;
    };

}