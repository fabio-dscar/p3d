#pragma once

#include <PhotonMath.h>
#include <Spectral.h>

namespace Photon {

    Color fresnelSchlick(Color refl, Float cosI);

    Float fresnelDielectric(Float eta, Float cosI);
    Float fresnelDielectric(Float etaInt, Float etaExt, Float cosI);
    Float fresnelDielectric(Float etaInt, Float etaExt, Float cosI, Float& cosT);

    Color fresnelConductor(const Color& eta, const Color& k, Float cosI);
}