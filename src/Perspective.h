#pragma once

#include <Camera.h>

namespace Photon {

    class Perspective : public Camera {
    public:
        Perspective(const Transform& objToWorld, Vec2ui res, Float fov, Float near, Float far);

        Color evalWe(const Ray& ray) const;
        Float pdfWe(const Ray& ray) const;
        Float pdfPosition(PositionSample* ps) const;

        Color sampleDirect(const Point2& rand, DirectSample* sample) const;
        Float pdfDirect(const DirectSample& sample) const;

    private:
        Float _filmArea;
    };
}