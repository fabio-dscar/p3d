#pragma once

#include <Light.h>

namespace Photon {

    class Shape;

    class AreaLight : public Light {
    public:
        AreaLight() : Light(), _shape(nullptr) { }
        AreaLight(std::shared_ptr<Shape> shape, const Color3& emission) 
            : Light(emission), _shape(shape) { }

        Color3 L(const RayEvent& evt, const Vec3& w) const;
        Float area() const;

        bool isDelta() const {
            return false;
        }

        void sampleLi(const RayEvent& ref, const Point2& rand, Point3* pos, Float* pdf) const;

    private:
        //Color3 _Le;
        std::shared_ptr<Shape> _shape;
    };

}