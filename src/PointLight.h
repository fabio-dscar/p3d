#pragma once

#include <Vector.h>
#include <Light.h>
#include <Sampling.h>

namespace Photon {

    class PointLight : public Light {
    public:
        PointLight() : Light(Color(1)) { }
        PointLight(const Point3& pos) : Light(Color(1)), _pos(pos) { }
        PointLight(const Point3& pos, const Color& color) : Light(color, translate(pos.posVec())), _pos(pos) { }
        PointLight(const Transform& objToWorld, const Color& color) : Light(color, objToWorld), _pos(objToWorld(Point3(0, 0, 0))) {}

        bool isDelta() const;

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
        Point3 _pos;
    };

}