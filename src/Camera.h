#pragma once

#include <Vector.h>

#include <Ray.h>

namespace Photon {

    class Camera {
    public:
        Camera() : _width(DEFAULT_WIDTH), _height(DEFAULT_HEIGHT) { }

        Camera(const Vec3& pos, const Vec3& up, const Vec3& target, float fov, float hither, Vec2u res) :
            _pos(pos), _up(up), _target(target), _fov(fov), _near(hither), _width(res.x), _height(res.y) { 

            calculateBasis();
        }

        unsigned int getWidth() const {
            return _width;
        }

        unsigned int getHeight() const {
            return _height;
        }

        const Vec3& getPosition() const {
            return _pos;
        }

        void setPosition(const Vec3& pos) {
            _pos = pos;
        }
        
        void setUp(const Vec3& up) {
            _up = up;
        }

        const Vec3& getTarget() const {
            return _target;
        }

        void setTarget(const Vec3& target) {
            _target = target;
        }

        void setFov(float fov) {
            _fov = fov;
        }

        float getNear() const {
            return _near;
        }

        void setNear(float hither) {
            _near = hither;
        }

        void setResolution(const Vec2u& res) {
            _width = res.x;
            _height = res.y;
        }

        void calculateBasis() {
            _n = glm::normalize(_pos - _target);
            _u = glm::normalize(glm::cross(_up, _n));
            _v = glm::normalize(glm::cross(_n, _u));
            
            _h = 2.0f * glm::length(_pos - _target) * std::tan(glm::radians(_fov) / 2.0f);
            _w = _width / _height * _h;
        }

        const Vec3& getU() const {
            return _u;
        }

        const Vec3& getV() const {
            return _v;
        }

        const Vec3& getN() const {
            return _n;
        }

        float getW() const {
            return _w;
        }

        float getH() const {
            return _h;
        }

        Ray getPrimaryRay(int x, int y) const {
            Vec3 dir = -glm::length(_pos - _target) * _n 
                + _h * ((float)y / (float)_height - 0.5f) * _v
                + _w * ((float)x / (float)_width - 0.5f) * _u;

            dir = glm::normalize(dir);

            return Ray(_pos, dir, _near);
        }

    private:
        unsigned int _width;
        unsigned int _height;
        
        Vec3 _pos;
        Vec3 _target;
        Vec3 _up;
        
        float _fov;
        float _near;

        Vec3 _u, _v, _n;
        float _h;
        float _w;
    };

}