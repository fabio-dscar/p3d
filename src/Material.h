#pragma once

#include <memory>

#include <Vector.h>
#include <PointLight.h>
#include <Ray.h>

namespace Photon {

    class Material {
    public:
        Material() : _color(1.0f), _diff(1.0f), _spec(1.0f), _shininess(1.0f) { }
        Material(const Color3& color, float diff, float spec, float shininess, float transmit, float ior) : 
            _color(color), _diff(diff), _spec(spec), _shininess(shininess), _transmit(transmit), _ior(ior) { }

        const Vec3& getColor() const {
            return _color;
        }

        float getDiff() const {
            return _diff;
        }

        float getSpec() const {
            return _spec;
        }

        float getShininess() const {
            return _shininess;
        }

        float getTransmit() const {
            return _transmit;
        }

        float getIor() const {
            return _ior;
        }

        bool isReflector() const {
            return (_spec > 0.0f);
        }

        bool isTransmissive() const {
            return (_transmit > 0.0f);
        }

        Color3 calcRadiance(const std::shared_ptr<Light>& light, const SurfaceEvent& surfInfo) const {
            Color3 color = Vec3(0.0f);

            Vec3 l = glm::normalize(light->getPosition() - surfInfo.point());
            float NdotL = glm::dot(surfInfo.normal(), l);

            // Reflected direction
            Vec3 r = glm::normalize(2.0f * NdotL * surfInfo.normal() - l);

            float RdotV = 0;
            if (glm::dot(r, surfInfo.wo()) > 0)
                RdotV = std::pow(glm::dot(r, surfInfo.wo()), _shininess);

            color += NdotL * _diff * _color * light->getColor();
            color += RdotV * _spec * _color * light->getColor();

            return color;
        }

    private:
        Color3 _color;
        float _diff;
        float _spec;
        float _transmit;
        float _shininess;
        float _ior;
    };

}