#pragma once

#include <Material.h>
#include <Ray.h>
#include <Bounds.h>

namespace Photon {

    class Geometry {
    public:
        Geometry() : _material() { }

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

        virtual Bounds3 bounds() const {
            return Bounds3(Point3(0));
        }

    private:
        Material _material;
    };

}