#pragma once

#include <Vector.h>
#include <Light.h>

namespace Photon {

    class PointLight : public Light {
    public:
        PointLight() : Light() { }
        PointLight(const Point3& pos) : Light(pos) { }
        PointLight(const Point3& pos, const Color3& color) : Light(pos, color) { }

        virtual bool isDelta() const {
            return true;
        }

        void sampleLi(const RayEvent& ref, const Point2& rand, Point3* pos, Float* pdf) const {
            *pos = _pos;
            *pdf = 1;
        }
    };

}