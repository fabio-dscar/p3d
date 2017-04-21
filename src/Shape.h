#pragma once

#include <Material.h>
#include <Ray.h>
#include <Bounds.h>
#include <Records.h>

namespace Photon {

    // Forward declaration
    class Light;
    class Material;
    class AreaLight;
    class BSDF;

    /*class PositionSample {
    public:
        Point3 pos;
        Float  pdf;
        const RayEvent* ref;

        PositionSample() : pdf(0), ref(nullptr) { }
        PositionSample(const RayEvent& ref) : pdf(0), ref(&ref) { }
    };*/

    class Shape {
    public:
        Shape() : _light(nullptr), _material() { }
        Shape(const AreaLight* light) : _light(light), _material() { }

        void addMaterial(const Material& material) {
            _material = material;
        }

        const Material& getMaterial() const {
            return _material;
        }

        const void setBsdf(const BSDF* bsdf) {
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
        BSDF const* _bsdf;
        Material _material;
        AreaLight const* _light;
    };

}