#include <Lambertian.h>

#include <Records.h>
#include <Sampling.h>

using namespace Photon;

Lambertian::Lambertian(const Color& rho)
    : BSDF(BSDFType(BSDFType::DIFFUSE |
                    BSDFType::REFLECTION)) {

    _kd = std::make_shared<ConstTexture<Color>>(rho);
}

Lambertian::Lambertian(const std::shared_ptr<Texture<Color>>& diffuseTex)
    : BSDF(BSDFType(BSDFType::DIFFUSE |
                    BSDFType::REFLECTION)) {

    _kd = diffuseTex;
}

Color Lambertian::eval(const BSDFSample& sample) const {
    if ((sample.wi.z * sample.evt->wo().z) > 0)
        return INVPI * _kd->fetch(sample.evt->uv());

    return Color::BLACK;
}