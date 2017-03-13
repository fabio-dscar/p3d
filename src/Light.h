#pragma once

namespace Photon {

    class Light {
    public:
        Light() : _pos(0.0f), _color(1.0f) {}
        Light(const Vec3& pos) : _pos(pos), _color(1.0f) {}
        Light(const Vec3& pos, const Color3& color) : _pos(pos), _color(color) {}

        void setPosition(const Vec3& pos) {
            _pos = pos;
        }

        const Vec3& getPosition() const {
            return _pos;
        }

        void setColor(const Color3& color) {
            _color = color;
        }

        const Color3& getColor() const {
            return _color;
        }

    private:
        Vec3 _pos;
        Color3 _color;
    };

}