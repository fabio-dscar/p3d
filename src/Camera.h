#pragma once

#include <vector>

#include <PhotonMath.h>
#include <Ray.h>
#include <Sampling.h>
#include <Film.h>

namespace Photon {

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
            _v = cross(_n, _u);

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

        Ray getPrimaryRayDOF(Float x, Float y, const Point2& rand) const {
            const Float focalDist = 2.0;
            const Float viewPlane = 2.9;

            Float tx = _w * ((Float)x / (Float)_film.width() - 0.5f);
            Float ty = _h * ((Float)y / (Float)_film.height() - 0.5f);

            Float px = tx * focalDist / viewPlane;
            Float py = ty * focalDist / viewPlane;

            Point2 diskPt = sampleUniformDisk(rand);
            diskPt *= 0.2;

            Vec3 dir = (px - diskPt.x) * _u + (py - diskPt.y) * _v - focalDist * _n;
            dir = normalize(dir);

            // Create primary ray
            Ray r(_pos + diskPt.x * _u + diskPt.y * _v, dir, _near);
            r.setPrimary(true);

            return r;
        }

        Ray getPrimaryRayDOF(const Point2& pixel, const Point2& rand) const {
            return getPrimaryRayDOF(pixel.x, pixel.y, rand);
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