#pragma once

#include <memory>
#include <iostream>

#include <Vector.h>
#include <PointLight.h>
#include <Ray.h>
#include <Spectral.h>

//#include <BSDF.h>
//#include <Texture.h>

namespace Photon {

    struct ShadingInfo {
        Color  ks;
        Color  kd;
        Normal n;
        Float  alpha;
        Float  eta;
    };

    /*class Material {
    public:
        Material() : _kd(nullptr), _bump(nullptr) { }

        Color sampleBSDF(BSDFType type, const Point2& rand, const SurfaceEvent& evt, BSDFSample* sample) const {
            if (_bsdfs.size() == 0)
                return Color::BLACK;

            // Sample the BSDF
            _bsdfs[0]->sampleF(evt.wo(), rand, sample);

            if (sample->pdf == 0.0)
                return Color::BLACK;

            // Compute shading info and evaluate BSDF
            ShadingInfo info = computeShadingInfo(evt);
            sample->f = _bsdfs[0]->evalF(sample->wi, evt.wo(), info);

            return sample->f;
        }

        Color evalBSDF(BSDFType type, const Vec3& wi, const SurfaceEvent& evt, const ShadingInfo& info) const {
            if (_bsdfs.size() == 0)
                return Color::BLACK;

            // Compute shading info and evaluate BSDF
            return _bsdfs[0]->evalF(wi, evt.wo(), info);
        }

        Float pdfBSDF() const {

        }

        ShadingInfo computeShadingInfo(const SurfaceEvent& evt) const {
            ShadingInfo info;
            
            // Diffuse map
            if (_kd)
                info.kd = _kd->fetch(evt);

            // Specular map
            if (_ks)
                info.ks = _ks->fetch(evt);

            // Roughness map
            if (_rough)
                info.alpha = _rough->fetch(evt);

            // Bump map
            if (_bump) {
                
            }

            return info;
        }

        void addBSDF(const BSDF* bsdf) {
            _bsdfs.push_back(bsdf);
        }

    protected:
        // Textures, etc
        std::shared_ptr<Texture<Color>> _kd;
        std::shared_ptr<Texture<Color>> _ks;
        std::shared_ptr<Texture<Float>> _bump;
        std::shared_ptr<Texture<Float>> _rough;

        std::vector<const BSDF*> _bsdfs;
    };*/

    class Material {
    public:
        Material() : _color(1.0f), _diff(1.0f), _spec(1.0f), _shininess(1.0f) { }
        Material(const Color& color, Float diff, Float spec, Float shininess, Float transmit, Float ior) :
            _color(color), _diff(diff), _spec(spec), _shininess(shininess), _transmit(transmit), _ior(ior) { }

        const Color& getColor() const {
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

        Color calcRadiance(const Vec3& wi, const SurfaceEvent& surfInfo) const {
            Color color = Color(0);

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
        Color  _color;
        Float  _diff;
        Float  _spec;
        Float  _transmit;
        Float  _shininess;
        Float  _ior;
    };

}