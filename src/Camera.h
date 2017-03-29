#pragma once

#include <vector>

#include <MathDefs.h>
#include <Ray.h>

namespace Photon {

    class Film {
    public:
        Film(uint32 width, uint32 height) 
            : _width(width), _height(height) { 

            _film.resize(_width * _height * 3);
            /*_normal.resize(_width * _height * 3);
            _depth.resize(_width * _height);*/
        }

        Film(const Vec2ui& res)
            : _width(res.x), _height(res.y) { 
        
            _film.resize(_width * _height * 3);
            /*_normal.resize(_width * _height * 3);
            _depth.resize(_width * _height);*/
        }

        uint32 width() const {
            return _width;
        }

        uint32 height() const {
            return _height;
        }

        Float aspect() const {
            return Float(_width) / Float(_height);
        }

        uint32 pixelArea() const {
            return _width * _height;
        }

        void addColorSample(uint32 x, uint32 y, const Color3& color) {
            uint32 idx = x + _width * y;

            _film[3*idx] = color.x;
            _film[3*idx+1] = color.y;
            _film[3*idx+2] = color.z;
        }

        void addNormalSample(uint32 x, uint32 y, const Vec3& normal) {
            uint32 idx = x + _width * y;

            /*_normal[3 * idx] = normal.x;
            _normal[3 * idx + 1] = normal.y;
            _normal[3 * idx + 2] = normal.z;*/
        }

        void addDepthSample(uint32 x, uint32 y, Float dist) {
            //_depth[x + _width * y] = dist;
        }

        const std::vector<Float>& image() const {
            return _film;
        }

        /*const Float* image(uint32 row) const {
            uint32 idx = _width * row;

            return &_film[idx * 3];
        }*/

        Color3 operator()(uint32 x, uint32 y) const {
            uint32 idx = x + _width * y;

            return Color3(_film[3*idx], _film[3*idx+1], _film[3*idx+2]);
        }

    private:
        uint32 _width, _height;
        std::vector<Float> _film;
        /*std::vector<float> _normal;
        std::vector<float> _depth;*/
    };

    class Camera {
    public:
        Camera() : _film(1280, 720) { }

        Camera(const Point3& pos, const Vec3& up, const Point3& target, Float fov, Float hither, Vec2ui res) :
            _pos(pos), _up(up), _target(target), _fov(fov), _near(hither), _film(res) { 

            calculateBasis();
        }

        uint32 width() const {
            return _film.width();
        }

        uint32 height() const {
            return _film.height();
        }

        const Point3& pos() const {
            return _pos;
        }

        void setPosition(const Point3& pos) {
            _pos = pos;
        }
        
        void setUp(const Vec3& up) {
            _up = up;
        }

        const Point3& target() const {
            return _target;
        }

        void setTarget(const Point3& target) {
            _target = target;
        }

        void fov(Float fov) {
            _fov = fov;
        }

        void setNear(Float hither) {
            _near = hither;
        }

        void calculateBasis() {
            _n = normalize(_pos - _target);
            _u = normalize(cross(_up, _n));
            _v = normalize(cross(_n, _u));

            /*
            Vec3 a, b;
            Math::basisFromVector(normalize(_pos - _target), &a, &b);
            a.normalize();
            b.normalize();

            Float r = Math::dot(a, b);
            Float d = Math::dot(_n, a);
            */

            _h = 2.0 * (_pos - _target).length() * std::tan(Math::radians(_fov) / 2.0);
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

        Float getW() const {
            return _w;
        }

        Float getH() const {
            return _h;
        }

        Ray getPrimaryRay(const Vec2ui pixel) const {
            return getPrimaryRay(pixel.x, pixel.y);
        }

        Ray getPrimaryRay(uint32 x, uint32 y) const {
            Vec3 dir = -(_pos - _target).length() * _n 
                + _h * ((Float)y / (Float)_film.height() - 0.5f) * _v
                + _w * ((Float)x / (Float)_film.width() - 0.5f) * _u;

            dir = normalize(dir);

            // Create primary ray
            Ray r(_pos, dir, _near);
            r.setPrimary(true);

            return r;
        }

        Ray getPrimaryRay(Float x, Float y) const {
            Vec3 dir = -(_pos - _target).length() * _n
                + _h * (y / (Float)_film.height() - 0.5f) * _v
                + _w * (x / (Float)_film.width() - 0.5f) * _u;

            dir = normalize(dir);

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
        
        Point3 _pos;
        Point3 _target;
        Vec3 _up;
        
        Float _fov;
        Float _near;

        Vec3 _u, _v, _n;
        Float _h;
        Float _w;
    };

}