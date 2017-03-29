#pragma once

#include <MathDefs.h>

namespace Photon {

    class Light {
    public:
        Light() : _pos(0.0f), _color(1.0f) {}
        Light(const Point3& pos) : _pos(pos), _color(1.0f) {}
        Light(const Point3& pos, const Color3& color) : _pos(pos), _color(color) {}

        void setPosition(const Point3& pos) {
            _pos = pos;
        }

        const Point3& pos() const {
            return _pos;
        }

        void setColor(const Color3& color) {
            _color = color;
        }

        const Color3& color() const {
            return _color;
        }

    private:
        Point3 _pos;
        Color3 _color;
    };

}