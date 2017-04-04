#pragma once

#include <memory>
#include <iostream>

#include <Vector.h>
#include <PointLight.h>
#include <Ray.h>

namespace Photon {

    class Material {
    public:
        Material() : _color(1.0f), _diff(1.0f), _spec(1.0f), _shininess(1.0f) { }
        Material(const Color3& color, Float diff, Float spec, Float shininess, Float transmit, Float ior) :
            _color(color), _diff(diff), _spec(spec), _shininess(shininess), _transmit(transmit), _ior(ior) { }

        const Color3& getColor() const {
            return _color;
        }

        Float getDiff() const {
            return _diff;
        }

        Float getSpec() const {
            return _spec;
        }

        Float getShininess() const {
            return _shininess;
        }

        Float getTransmit() const {
            return _transmit;
        }

        Float getIor() const {
            return _ior;
        }

        bool isReflector() const {
            return (_spec > 0.0f);
        }

        bool isTransmissive() const {
            return (_transmit > 0.0f);
        }

        Color3 calcRadiance(const Vec3& wi, const SurfaceEvent& surfInfo) const {
            Color3 color = Color3(0);

            Vec3 l = normalize(wi);
            Float NdotL = dot(surfInfo.normal(), l);

            // Reflected direction
            Vec3 r = normalize(2.0f * NdotL * surfInfo.normal() - l);

            Float RdotV = 0;
            if (dot(r, surfInfo.wo()) > 0)
                RdotV = std::pow(dot(r, surfInfo.wo()), _shininess);

            color += NdotL * _diff * _color;
            color += RdotV * _spec * _color;

            return color;
        }

    private:
        Color3 _color;
        Float  _diff;
        Float  _spec;
        Float  _transmit;
        Float  _shininess;
        Float  _ior;
    };

}