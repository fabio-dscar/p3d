#pragma once

#include <Vector.h>
#include <Spectral.h>
#include <Records.h>
#include <Transform.h>

namespace Photon {

    class RayEvent;
    class Scene;

    class Light {
    public:
        Light() : _Le(1) {}
        Light(const Color& emission) : _Le(emission) {}
        Light(const Color& emission, const Transform& objToWorld)
            : _Le(emission), _objToWorld(objToWorld), _worldToObj(inverse(objToWorld)) {}

        void setIntensity(const Color& Le);
        const Color& emission() const;
        Color L(const RayEvent& evt, const Vec3& w) const;

        virtual void initialize(const Scene& scene);

        virtual uint32 numSamples() const;
        virtual bool isEnvironment() const;
        virtual bool isDelta() const;

        virtual Color power() const = 0;

        // Evaluate L for outgoing wo at intersection
        virtual Color evalL(const SurfaceEvent& it, const Vec3& wo) const = 0;
        virtual Color evalL(const PositionSample& sample, const Vec3& wo) const = 0;

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
        Color _Le;

        Transform _objToWorld;
        Transform _worldToObj;
    };

}