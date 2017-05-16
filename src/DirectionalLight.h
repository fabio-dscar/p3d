#pragma once

#include <Light.h>

namespace Photon {

    class DirectionalLight : public Light {
    public:


        Color power() const {

        }

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

    private:
        Vec3 _w;
    };

}