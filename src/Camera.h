#pragma once

#include <vector>

#include <PhotonMath.h>
#include <Ray.h>
#include <Sampling.h>
#include <Film.h>

#include <Transform.h>
#include <Sampler.h>

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

        void sampleDirection();
    protected:
        mutable Film _film;
        
        Transform _camToWorld;
        Transform _camToClip;
        Transform _planeToCam;
        Transform _camToPlane;
        
        Float _fov;
        Float _near;
        Float _far;

        Float _h;
        Float _w;

        Float _lensRad;
        Float _focalDist;      
    };

}