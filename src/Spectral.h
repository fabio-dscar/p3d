#pragma once

#include <PhotonMath.h>
#include <Vector.h>

namespace Photon {

    class SpectralRGB {
    public:
        Float r, g, b;

        SpectralRGB() : r(0), g(0), b(0) { }
        SpectralRGB(Float s) : r(s), g(s), b(s) {}
        SpectralRGB(Float r, Float g, Float b)
            : r(r), g(g), b(b) { }

        SpectralRGB  operator+ (const SpectralRGB& rgb) const;
        SpectralRGB& operator+=(const SpectralRGB& rgb);

        SpectralRGB  operator- (const SpectralRGB& rgb) const;
        SpectralRGB& operator-=(const SpectralRGB& rgb);

        SpectralRGB  operator* (Float scalar) const;
        SpectralRGB& operator*=(Float scalar);

        SpectralRGB  operator* (const SpectralRGB& rgb) const;
        SpectralRGB& operator*=(const SpectralRGB& rgb);

        SpectralRGB  operator/ (Float scalar) const;
        SpectralRGB& operator/=(Float scalar);

        SpectralRGB  operator/ (const SpectralRGB& scalar) const;
        SpectralRGB& operator/=(const SpectralRGB& scalar);

        Float  operator[](uint32 idx) const;
        Float& operator[](uint32 idx);

        Float max() const;
        Float min() const;

        void  clamp(Float low, Float high);
        bool  isBlack() const;
        Float lum() const;

        static const SpectralRGB BLACK;
    };

    inline SpectralRGB operator*(Float scalar, const SpectralRGB& rgb) {
        return rgb * scalar;
    }

    inline SpectralRGB operator/(Float scalar, const SpectralRGB& rgb) {
        return rgb / scalar;
    }

    /*inline SpectralRGB abs(const SpectralRGB& rgb) {
        return SpectralRGB(std::abs(rgb.r), std::abs(rgb.g), std::abs(rgb.b));
    }*/

    inline SpectralRGB clamp(const SpectralRGB& rgb, Float low, Float high) {
        Float r = Math::clamp<Float>(rgb.r, low, high);
        Float g = Math::clamp<Float>(rgb.g, low, high);
        Float b = Math::clamp<Float>(rgb.b, low, high);
        return SpectralRGB(r, g, b);
    }

    inline SpectralRGB sqrtSafe(const SpectralRGB& rgb) {
        Float r = Math::sqrtSafe(rgb.r);
        Float g = Math::sqrtSafe(rgb.g);
        Float b = Math::sqrtSafe(rgb.b);
        return SpectralRGB(r, g, b);
    }

    inline SpectralRGB exp(const SpectralRGB& rgb) {
        Float r = std::exp(rgb.r);
        Float g = std::exp(rgb.g);
        Float b = std::exp(rgb.b);
        return SpectralRGB(r, g, b);
    }

    inline SpectralRGB lerp(const SpectralRGB& rgb1, const SpectralRGB& rgb2, Float t) {
        return (1.0 - t) * rgb1 + t * rgb2;
    }

    /* ----------------------------------------------------------
          1931 CIE XYZ Color Matching Function
          http://cvrl.ioo.ucl.ac.uk/cmfs.htm
    ---------------------------------------------------------*/
    static PHOTON_CONSTEXPR uint32 CIENumSamples  = 471;
    static PHOTON_CONSTEXPR Float  CIELambdaStart = 360.0;
    static PHOTON_CONSTEXPR Float  CIELambdaEnd   = 830.0;

    extern const Float CIESamplesX[];
    extern const Float CIESamplesY[];
    extern const Float CIESamplesZ[];
    
    inline SpectralRGB XYZToRGB(SpectralRGB xyz) {
        return SpectralRGB(
            3.240479 * xyz.r + -1.537150 * xyz.g + -0.498535 * xyz.b,
           -0.969256 * xyz.r +  1.875991 * xyz.g +  0.041556 * xyz.b,
            0.055648 * xyz.r + -0.204043 * xyz.g +  1.057311 * xyz.b
        );
    }

    inline SpectralRGB RGBToXYZ(SpectralRGB rgb) {
        return SpectralRGB(
            0.412453 * rgb.r + 0.357580 * rgb.g + 0.180423 * rgb.b,
            0.212671 * rgb.r + 0.715160 * rgb.g + 0.072179 * rgb.b,
            0.019334 * rgb.r + 0.119193 * rgb.g + 0.950227 * rgb.b
        );
    }

    typedef SpectralRGB Color;
}