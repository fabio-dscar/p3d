#pragma once

#include <Vector.h>

namespace Photon {

    class Camera {
    public:
        Camera() : _width(DEFAULT_WIDTH), _height(DEFAULT_HEIGHT) { }

        Camera(const Vec3& pos, const Vec3& up, const Vec3& target, float fov, float hither) :
            _pos(pos), _up(up), _target(target), _fov(fov), _near(hither) { }

        unsigned int getWidth() const {
            return _width;
        }

        unsigned int getHeight() const {
            return _height;
        }

        void setPosition(const Vec3& pos) {
            _pos = pos;
        }
        
        void setUp(const Vec3& up) {
            _up = up;
        }

        void setTarget(const Vec3& target) {
            _target = target;
        }

        void setFov(float fov) {
            _fov = fov;
        }

        void setNear(float hither) {
            _near = hither;
        }

        void setResolution(const Vec2u& res) {
            _width = res.x;
            _height = res.y;
        }
    
    private:
        unsigned int _width;
        unsigned int _height;
        
        Vec3 _pos;
        Vec3 _target;
        Vec3 _up;
        
        float _fov;
        float _near;
    };

}