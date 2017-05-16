#pragma once

#include <Ray.h>
#include <Bounds.h>
#include <Records.h>
#include <Transform.h>

namespace Photon {

    // Forward declaration
    class Light;
    class AreaLight;
    class BSDF;

    class Shape {
    public:
        Shape() : _light(nullptr), _bsdf(nullptr), _twoSided(false) { }
        Shape(const AreaLight* light) : _light(light), _bsdf(nullptr), _twoSided(false) { }

        void setTransform(const Transform& objToWorld) {
            _objToWorld = objToWorld;
            _worldToObj = inverse(_objToWorld);
        }

        void setTransform(const Transform& objToWorld, const Transform& worldToObj) {
            _objToWorld = objToWorld;
            _worldToObj = worldToObj;
        }

        void setBsdf(const BSDF* bsdf) {
            _bsdf = bsdf;
        }

        const BSDF* bsdf() const {
            return _bsdf;
        }

        virtual bool intersectRay(const Ray& ray, SurfaceEvent* evt) const {
            return false;
        }

        virtual bool isOccluded(const Ray& ray) const {
            return false;
        }

        virtual Bounds3 bbox() const {
            return Bounds3(Point3(0));
        }

        virtual Float area() const {
            return 0;
        }

        bool isLight() const {
            return _light != nullptr;
        }

        void setLight(AreaLight* light) {
            _light = light;
        }

        AreaLight const* areaLight() const {
            return _light;
        }

        virtual void computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const {
            if (_twoSided) {
                evt.normal *= sign(Frame::cosTheta(evt.wo));
                evt.gFrame  = Frame(evt.normal);
            }
        }

        virtual void  samplePosition(const Point2& rand, PositionSample* sample) const {
            sample->pos = Point3(0);
        }

        virtual Float pdfPosition(const PositionSample& sample) const {
            return 0;
        }

        virtual void  sampleDirect(const Point2& rand, DirectSample* sample) const {
            sample->wi = Vec3(0);
        }

        virtual Float pdfDirect(const DirectSample& sample) const {
            return 0;
        }

    private:
        const BSDF* _bsdf;
        const AreaLight* _light;

        Transform _objToWorld;
        Transform _worldToObj;   

        bool _twoSided;
    };

}