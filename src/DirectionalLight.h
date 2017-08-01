#pragma once

#include <Light.h>

namespace Photon {

    class DirectionalLight : public Light {
    public:
        DirectionalLight();
        DirectionalLight(const Transform& objToWorld, const Color& color);

        bool isDelta() const;
        Color power() const;

        void initialize(const Scene& scene);

        // Evaluate L for outgoing wo at intersection
        Color evalL(const SurfaceEvent& it, const Vec3& wo) const;
        Color evalL(const PositionSample& sample, const Vec3& wo) const;

        // Sample a position on the surface
        Color samplePosition(const Point2& rand, PositionSample* sample) const;
        Float pdfPosition(const PositionSample& sample) const;

        // Sample an incoming direction to a distant reference position
        Color sampleDirect(const Point2& rand, DirectSample* sample) const;
        Float pdfDirect(const DirectSample& sample) const;

        // Sample an outgoing direction from a local reference position
        Color sampleEmitDirection(const Point2& rand, const PositionSample& pos, DirectionSample* sample) const ;
        Float pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const;

    private:
        Vec3 _w;

        Point3 _sceneCenter;
        Float _sceneRadius;
    };

}