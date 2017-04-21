#pragma once

#include <Material.h>

namespace Photon {

    class Matte : public Material {
    public:
        enum MatteBSDF {
            DIFFUSE = 0,
            ROUGH_DIFFUSE = 1
        };

        Matte(std::shared_ptr<Texture<Color>> kd, std::shared_ptr<Texture<Float>> rough) {
            _kd = kd;
            _rough = rough;

            // Default BSDFs
            
        }

        Matte(std::shared_ptr<Texture<Color>> kd, std::shared_ptr<Texture<Float>> rough, const BSDF* bsdf) {
            _kd = kd;
            _rough = rough;
            _bsdfs.push_back(bsdf);
        }

        Color sampleBSDF(BSDFType type, const Point2& rand, const SurfaceEvent& evt, BSDFSample* sample) const {
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

    };

}