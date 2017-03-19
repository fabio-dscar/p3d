#pragma once

#include <vector>

#include <MathDefs.h>
#include <Vector.h>
#include <Ray.h>

namespace Photon {

    class Film {
    public:
        Film(uint32 width, uint32 height) 
            : _width(width), _height(height) { 

            _film.resize(_width * _height * 3);
        }

        Film(const Vec2u& res)
            : _width(res.x), _height(res.y) { 
        
            _film.resize(_width * _height * 3);
        }

        uint32 width() const {
            return _width;
        }

        uint32 height() const {
            return _height;
        }

        float aspect() const {
            return (float)_width / (float)_height;
        }

        void addColorSample(uint32 x, uint32 y, Color3 color) {
            uint32 idx = x + _height * y;

            _film[3*idx] = color.r;
            _film[3*idx+1] = color.g;
            _film[3*idx+2] = color.b;
        }

        const std::vector<float>& image() const {
            return _film;
        }

        const float* image(uint32 row) const {
            uint32 idx = _width * row;

            return &_film[idx * 3];
        }

        Color3 operator()(uint32 x, uint32 y) const {
            uint32 idx = x + _height * y;

            return Color3(_film[3*idx], _film[3*idx+1], _film[3*idx+2]);
        }

    private:
        uint32 _width, _height;
        std::vector<float> _film;
    };


    class Camera {
    public:
        Camera() : _film(DEFAULT_WIDTH, DEFAULT_HEIGHT) { }

        Camera(const Vec3& pos, const Vec3& up, const Vec3& target, float fov, float hither, Vec2u res) :
            _pos(pos), _up(up), _target(target), _fov(fov), _near(hither), _film(res) { 

            calculateBasis();
        }

        uint32 getWidth() const {
            return _film.width();
        }

        uint32 getHeight() const {
            return _film.height();
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

        void calculateBasis() {
            _n = glm::normalize(_pos - _target);
            _u = glm::normalize(glm::cross(_up, _n));
            _v = glm::normalize(glm::cross(_n, _u));
            
            _h = 2.0f * glm::length(_pos - _target) * std::tan(glm::radians(_fov) / 2.0f);
            _w = _film.width() / _film.height() * _h;
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

        Ray getPrimaryRay(const Vec2u pixel) const {
            return getPrimaryRay(pixel.x, pixel.y);
        }

        Ray getPrimaryRay(int x, int y) const {
            Vec3 dir = -glm::length(_pos - _target) * _n 
                + _h * ((float)y / (float)_film.height() - 0.5f) * _v
                + _w * ((float)x / (float)_film.width() - 0.5f) * _u;

            dir = glm::normalize(dir);

            // Create primary ray
            Ray r(_pos, dir, _near);
            r.setPrimary(true);

            return r;
        }

        Film& film() const {
            return _film;
        }

    private:
        mutable Film _film;
        
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