#pragma once

#include <BSDF.h>
#include <Records.h>

namespace Photon {

    class Phong : public BSDF {
    public:
        Phong(Float alpha) : BSDF(BSDFType::REFLECTION), _alpha(alpha) { 
            _factor = (alpha + 2.0) * INV2PI;
        }

        Float evalPdf(const BSDFSample& sample) const {

        }

        Color eval(const BSDFSample& sample) const {
            Float phong = 0;

            if (sample.wi.z <= 0.0f || sample.evt->wo().z <= 0.0f)
                return Color::BLACK;

            if (hasType(sample.type, BSDFType::DIFFUSE))
                phong += INVPI * _diffRatio;

            if (hasType(sample.type, BSDFType::GLOSSY)) {
                float cosTheta = Vec3(-event.wi.x(), -event.wi.y(), event.wi.z()).dot(event.wo);
                if (cosTheta > 0)
                    phong += _factor * std::pow(cosTheta, _alpha) * (1.0f - _diffRatio);
            }

            return phong * _kd->fetch(*sample.evt);
        }

        Color sampleF(const Point2& rand, BSDFSample* sample) const {

        }

    private:
        std::shared_ptr<Texture<Color>> _kd;
        Float _alpha;
        Float _factor;
        Float _diffRatio;
    };

}