#pragma once

#include <Vector.h>
#include <Light.h>

namespace Photon {

    class PointLight : public Light {
    public:
        PointLight() : Light() { }
        PointLight(const Vec3& pos) : Light(pos) { }
        PointLight(const Vec3& pos, const Color3& color) : Light(pos, color) { }

    };

}