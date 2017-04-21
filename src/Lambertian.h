#pragma once

#include <BSDF.h>
#include <ConstTexture.h>

namespace Photon {

    class BSDFSample;

    // Represents lambertian reflection
    class Lambertian : public BSDF {
    public:
        Lambertian(const Color& rho);
        Lambertian(const std::shared_ptr<Texture<Color>>& diffuseTex);

        Color eval(const BSDFSample& sample) const;
 
    private:
        std::shared_ptr<Texture<Color>> _kd;
    };

}