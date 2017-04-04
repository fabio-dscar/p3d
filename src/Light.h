#pragma once

#include <Vector.h>

namespace Photon {

    class RayEvent;

    class Light {
    public:
        Light() : _pos(0), _Le(1.0) {}
        Light(const Color3& emission) : _Le(emission) { }
        Light(const Point3& pos) : _pos(pos), _Le(1.0) {}
        Light(const Point3& pos, const Color3& color) : _pos(pos), _Le(color) {}

        void setPosition(const Point3& pos);
        const Point3& pos() const;
        void setColor(const Color3& color);
        const Color3& color() const;
        Color3 L(const RayEvent& evt, const Vec3& w) const;

        virtual bool isDelta() const = 0;
        virtual void sampleLi(const RayEvent& ref, const Point2& rand, Point3* pos, Float* pdf) const = 0;

    protected:
        Point3 _pos;
        Color3 _Le;
    };

}