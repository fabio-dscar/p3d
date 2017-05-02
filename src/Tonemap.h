#pragma once

#include <Spectral.h>

namespace Photon {

    static const Float GAMMA = 2.2; // 2.4

    enum ToneOperator {
        LINEAR,
        LUM_REINHARD,
        PHYSI_REINHARD,
        FILMIC
    };

    inline Float applyGamma(Float val) {
        if (val <= 0.0031308)
            return 12.92 * val;

        return 1.055 * std::pow(val, (1.0 / GAMMA)) - 0.055;
    }

    inline Float inverseGamma(Float val) {
        if (val <= 0.04045)
            return val * 1.0 / 12.92;

        return std::pow((val + 0.055) * 1.0 / 1.055, GAMMA);
    }

    inline SpectralRGB applyGamma(const SpectralRGB& color) {
        SpectralRGB ret;
        for (uint32 i = 0; i < 3; i++)
            ret[i] = applyGamma(color[i]);
        return ret;
    }

    inline SpectralRGB inverseGamma(const SpectralRGB& color) {
        SpectralRGB ret;
        for (uint32 i = 0; i < 3; i++)
            ret[i] = inverseGamma(color[i]);
        return ret;
    }

    inline SpectralRGB filmicHDR(SpectralRGB in, Float exposure);

    static inline SpectralRGB ToneMap(ToneOperator type, SpectralRGB color, Float exposure) {
        switch (type) {
            case FILMIC:
                return filmicHDR(color, exposure);
                break;
            case LUM_REINHARD:
            case PHYSI_REINHARD:
                // Performed as post-processing
                break;
            case LINEAR:
            default:
                //return color;
                return applyGamma(color);
                break;
        }

        return Color::BLACK;
    }

    inline SpectralRGB filmicHDR(SpectralRGB in, Float exposure) {
        //const Float EXPOSURE = 0.6; // 0.6

        in = exposure * in;

        Float a = 2.51f;
        Float b = 0.03f;
        Float c = 2.43f;
        Float d = 0.59f;
        Float e = 0.14f;
        SpectralRGB nom = (in * (a * in + SpectralRGB(b)));
        SpectralRGB denom = (in * (c * in + SpectralRGB(d)) + SpectralRGB(e));

        SpectralRGB res = clamp(SpectralRGB(nom.r / denom.r, nom.g / denom.g, nom.b / denom.b), 0, 1);

        return applyGamma(res);
    }

}