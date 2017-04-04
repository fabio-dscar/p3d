#pragma once

#include <Material.h>
#include <Ray.h>
#include <Bounds.h>

namespace Photon {

    // Forward declaration
    class Light;
    class Material;
    class AreaLight;

    class Shape {
    public:
        Shape() : _light(nullptr), _material() { }
        Shape(AreaLight* light) : _light(light), _material() { }

        void addMaterial(const Material& material) {
            _material = material;
        }

        const Material& getMaterial() const {
            return _material;
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

        virtual RayEvent samplePosition(const RayEvent& ref, const Point2& rand, Float* pdf) const {
            return RayEvent();
        }

        virtual Float pdfPosition(const RayEvent& evt) const {
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

    private:
        Material _material;
        AreaLight const* _light;
    };

}