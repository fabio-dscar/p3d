#pragma once

#include <vector>

#include <PhotonMath.h>
#include <Ray.h>
#include <Sampling.h>
#include <Film.h>

#include <Transform.h>
#include <Sampler.h>

#include <Records.h>

namespace Photon {

    class Camera {
    public:
        Camera() : _film(1280, 720) { }
        Camera(const Transform& camToWorld, Vec2ui res, Float near, Float far);
        
        uint32 width() const {
            return _film.width();
        }

        uint32 height() const {
            return _film.height();
        }

        void fov(Float fov) {
            _fov = fov;
        }

        void setNear(Float hither) {
            _near = hither;
        }

        Ray primaryRay(const Point2ui& pixel, Sampler& sampler) const;
        Ray primaryRay(const Point2& pixel, const Point2& lens) const;

        Film& film() const {
            return _film;
        }

        Frame camFrame() const;

        static Transform lookAt(const Point3& pos, const Point3& at, const Vec3& up);

        void setLensParams(Float radius, Float focalDist);

        bool toRaster(const Ray& ray, Point2* raster) const {
            Float cos = dot(ray.dir(), _n);
            if (cos <= 0)
                return false;

            Float t = 1.0 / cos;
            if (_lens.radius > 0)
                t *= _lens.focalDist;

            Point3 focus = ray(t);
            Point3 vplane = _worldToPlane(focus);

            // Check if point lies in view plane
            Point2 p = Point2(vplane.x, vplane.y);
            if (!_film.bounds().contains(p))
                return false;

            *raster = p;

            return true;
        }

        virtual Float pdfPosition(PositionSample* ps) const = 0;

        virtual Color evalWe(const Ray& ray) const = 0;
        virtual Float pdfWe(const Ray& ray) const = 0;

        virtual Color sampleDirect(const Point2& rand, DirectSample* sample) const = 0;
        virtual Float pdfDirect(const DirectSample& sample) const = 0;

    protected:
        mutable Film _film;
        
        Transform _camToWorld;
        //Transform _camToClip;
        Transform _planeToCam;
        //Transform _camToPlane;

        Transform _worldToPlane;
        
        Float _fov;
        Float _near;
        Float _far;

        Normal _n;

        struct Lens {
            Float radius;
            Float focalDist;
            Float area;
        } _lens;
    };

}