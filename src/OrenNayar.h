#pragma once

#include <BSDF.h>
#include <Sampling.h>
#include <ConstTexture.h>
#include <Records.h>

namespace Photon {

    // Represents rough diffuse reflection
    class OrenNayar : public BSDF {
    public:
        OrenNayar(const Color& rho, Float sigma);
        OrenNayar(const std::shared_ptr<Texture<Color>>& diffuseTex, Float sigma);

        Color eval(const BSDFSample& sample) const;

    private:
        std::shared_ptr<Texture<Color>> _kd;
        Float _sigma;
    };

}