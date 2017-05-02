#include <Microfacet.h>

#include <Frame.h>

using namespace Photon;

MicrofacetDist::MicrofacetDist(DistributionType type, const Vec2& alpha)
    : _type(type), _alphaUV(alpha), _phongExp(0) {

    if (_type == PHONG) {
        _phongExp.x = Math::max(2.0 / (alpha.x * alpha.x) - 2.0, 0.0);
        _phongExp.y = Math::max(2.0 / (alpha.y * alpha.y) - 2.0, 0.0);
    }

    _alphaUV.x = unitToAlpha(alpha.x);
    _alphaUV.y = unitToAlpha(alpha.y);

    _alphaUV.x = std::max(_alphaUV.x, (Float)1e-5);
    _alphaUV.y = std::max(_alphaUV.y, (Float)1e-5);
}

DistributionType MicrofacetDist::type() const {
    return _type;
}

Float MicrofacetDist::evalD(const Vec3& wh) const {
    if (Frame::cosTheta(wh) <= 0) // Heaviside
        return 0;

    switch (_type) {
        case PHONG:
            return distPhong(wh, _alphaUV, _phongExp);
        case BECKMANN:
            return distBeckmann(wh, _alphaUV);
        case GGX:
            return distGGX(wh, _alphaUV);
        default:
            return 0;
    }
}

Float MicrofacetDist::evalLambda(const Vec3& w) const {
    switch (_type) {
        case PHONG:
        case BECKMANN:
            return lambdaBeckmann(w, _alphaUV);
        case GGX:
            return lambdaGGX(w, _alphaUV);
        default:
            return 0;
    }
}

Float MicrofacetDist::evalG1(const Vec3& w, const Vec3& wh) const {
    if (Frame::cosTheta(w) * dot(w, wh) <= 0)
        return 0;

    return 1.0 / (1.0 + evalLambda(w));
}

Float MicrofacetDist::evalG(const Vec3& wi, const Vec3& wo, const Vec3& wh) const {
    if (Frame::cosTheta(wi) * dot(wi, wh) <= 0)
        return 0;

    return 1.0 / (1.0 + evalLambda(wi) + evalLambda(wo));
}

Float MicrofacetDist::evalPdf(const Vec3& wi, const Vec3& wh) const {
    if (_type == DistributionType::PHONG)
        return evalD(wh) * Frame::cosTheta(wh);

    // Visible normal sampling pdf
    return evalD(wh) * evalG1(wi, wh) * Math::absDot(wi, wh) / Frame::absCosTheta(wi);
}

Normal MicrofacetDist::sample(const Vec3& wi, const Point2& rand) const {
    if (_type == DistributionType::PHONG)
        return samplePhong(wi, rand);
    
    return sampleVisible(wi, rand);
}

Float Photon::interpolateAlpha(const Vec3& w, const Vec2& alphaUV) {
    if (alphaUV.x == alphaUV.y)
        return alphaUV.x;  // Isotropic

    Vec2 alpha2 = alphaUV * alphaUV;
    Vec2 hyp2   = Vec2(Frame::cosPhiSqr(w), Frame::sinPhiSqr(w));

    return std::sqrt(dot(alpha2, hyp2));
}

Float Photon::distPhong(const Vec3& wh, const Vec2& alphaUV, const Vec2& phongExp) {
    Float exponent = 0;

    return std::sqrt((phongExp.x + 2) * (phongExp.y + 2))
        * INV2PI * std::pow(Frame::cosTheta(wh), exponent);
}

Float Photon::distPhong(const Vec3& wh, const Vec2& alphaUV) {
    Vec2 phongExp;
    phongExp.x = Math::max(2.0 / (alphaUV.x * alphaUV.x) - 2.0, 0.0);
    phongExp.y = Math::max(2.0 / (alphaUV.y * alphaUV.y) - 2.0, 0.0);

    return distPhong(wh, alphaUV, phongExp);
}

Float Photon::distBeckmann(const Vec3& wh, const Vec2& alphaUV) {
    Float cos2  = Frame::cosThetaSqr(wh);
    Float exp   = computeBeckmannExp(wh, alphaUV);
    Float denom = PI * alphaUV.x * alphaUV.y * cos2 * cos2;

    return std::exp(-exp) / denom;
}

Float Photon::distGGX(const Vec3& wh, const Vec2& alphaUV) {
    Float exp   = computeBeckmannExp(wh, alphaUV);
    Float denom = (1.0 + exp) * Frame::cosThetaSqr(wh);

    return 1.0 / (PI * alphaUV.x * alphaUV.y * denom * denom);
}

Float Photon::computeBeckmannExp(const Vec3& wh, const Vec2& alphaUV) {
    Float cos2 = Frame::cosThetaSqr(wh);
    return ((wh.x * wh.x) / (alphaUV.x * alphaUV.x) +
            (wh.y * wh.y) / (alphaUV.y * alphaUV.y)) / cos2;
}

Float Photon::lambdaBeckmann(const Vec3& w, const Vec2& alphaUV) {
    if (Frame::cosTheta(w) == 0)
        return 0;

    Float alpha = interpolateAlpha(w, alphaUV);
    Float a = 1.0 / (alpha * abs(Frame::tanTheta(w)));
    if (a >= 1.6)
        return 0;

    return (1.0 - 1.259 * a + 0.396 * a * a) /
           (3.535 * a + 2.181 * a * a);
}

Float Photon::lambdaGGX(const Vec3& w, const Vec2& alphaUV) {
    Float cos2 = Frame::cosThetaSqr(w);
    if (cos2 == 0)
        return 0;

    // Interpolate alpha
    Float alpha = interpolateAlpha(w, alphaUV);
    Float sin2 = Frame::sinThetaSqr(w);
    Float tan2 = std::abs(sin2 / cos2);

    return (-1.0 + std::sqrt(1.0 + tan2 * alpha * alpha)) / 2.0;
}

Float MicrofacetDist::unitToAlpha(Float rough) {
    Float x = std::log(std::max((Float)1e-5, rough));
    Float x2 = x * x;
    return 1.62142 + 0.819955 * x + 0.1734 * x2 + 0.0171201 * x2 * x + 0.000640711 * x2 * x2;
}

Normal MicrofacetDist::samplePhong(const Vec3& wi, const Point2& rand) const {
    // Walter's trick
    Vec2 alpha = _alphaUV * (1.2 - 0.2 * std::sqrt(Frame::absCosTheta(wi)));

    // ...

    return Normal(0);
}

Normal MicrofacetDist::sampleVisible(const Vec3& wi, const Point2& rand) const {
    // Stretch wi
    Vec3 w = normalize(wi * Vec3(_alphaUV.x, _alphaUV.y, 1.0));

    // Get polar coordinates
    Float theta = 0;
    Float phi = 0;

    // Handle normal incidence
    if (Frame::cosTheta(w) < 0.9999) {
        theta = std::acos(Frame::cosTheta(w));
        phi = std::atan2(w.y, w.x);
    }

    Float sinPhi = std::sin(phi);
    Float cosPhi = std::cos(phi);

    // Simulate P22(slope.x, slope.y, 1, 1)
    Vec2 slope = sampleP22(theta, rand);

    // Rotate
    slope = Vec2(cosPhi * slope.x - sinPhi * slope.y,
                 sinPhi * slope.x + cosPhi * slope.y);
    slope *= _alphaUV; // Unstretch

    return normalize(Normal(-slope.x, -slope.y, 1.0));
}

Vec2 MicrofacetDist::sampleP22(Float theta, const Point2& rand) const {
    Point2 u = rand;
    Vec2 slope;

    switch (_type) {
        case BECKMANN:
        {
            if (theta < 0.0001) {
                const Float r = std::sqrt(-std::log(u.x));
                const Float phi = 6.28318530718 * u.y;
                return Vec2(r * std::cos(phi), r * std::sin(phi));
            }

            const Float sin = std::sin(theta);
            const Float cos = std::cos(theta);
            const Float tan = sin / cos;
            const Float a = 1.0 / tan;
            const Float erfA = Math::erf(a);
            const Float expA2 = std::exp(-a * a);
            const Float lambda = 0.5 * (erfA - 1.0) + 0.5 * SQRTINVPI * expA2 / a;
            const Float G1 = 1.0 / (1.0 + lambda);
            const Float C = 1.0 - G1 * erfA;

            if (u.x < C) {
                u.x = u.x / C; // Reuse sample

                const Float w1 = 0.5 * SQRTINVPI * sin * expA2;
                const Float w2 = cos * (0.5 - 0.5 * erfA);
                const Float p = w1 / (w1 + w2);

                if (u.x < p) {
                    u.x = u.x / p; // Reuse sample
                    slope.x = -std::sqrt(-std::log(u.x * expA2));
                } else {
                    u.x = (u.x - p) / (1.0 - p); // Reuse sample
                    slope.x = Math::erfInv(u.x - 1.0 - u.x * erfA);
                }


            } else {
                u.x = (u.x - C) / (1.0 - C); // Reuse sample

                slope.x = Math::erfInv((-1.0 + 2.0 * u.x) * erfA);

                const Float p = (-slope.x * sin + cos) / (2.0 * cos);
                if (u.y > p) {
                    slope.x = -slope.x;
                    u.y = (u.y - p) / (1.0 - p);
                } else {
                    u.y = u.y / p;
                }
            }

            slope.y = Math::erfInv(2.0 * u.y - 1.0);

            return slope;
        }
        break;
        case GGX:
        {
            // Handle normal incidence
            if (theta < 0.0001) {
                const Float r = std::sqrt(u.x / (1.0 - u.x));
                const Float phi = 6.28318530718 * u.y;
                return Vec2(r * std::cos(phi), r * std::sin(phi));
            }

            // Sample slope.x
            const Float tan = std::tan(theta);
            const Float a = 1.0 / tan;
            const Float G1 = 2.0 / (1.0 + std::sqrt(1.0 + 1.0 / (a * a)));
            const Float A = 2.0 * u.x / G1 - 1.0;
            const Float tmp = 1.0 / (A * A - 1.0);

            const Float D = Math::sqrtSafe(tan * tan * tmp * tmp - (A * A - tan * tan) * tmp);
            const Float slopeX1 = tan * tmp - D;
            const Float slopeX2 = tan * tmp + D;

            slope.x = slopeX2;
            if (A < 0 || slopeX2 >(1.0 / tan))
                slope.x = slopeX1;

            // Sample slope.y
            Float S;
            if (u.y > 0.5) {
                S = 1.0;
                u.y = 2.0 * (u.y - 0.5); // Reuse sample
            } else {
                S = -1.0;
                u.y = 2.0 * (0.5 - u.y); // Reuse sample
            }

            const Float z = (u.y * (u.y * (u.y * 0.27385 - 0.73369) + 0.46341)) /
                (u.y * (u.y * (u.y * 0.093073 + 0.309420) - 1.0) + 0.597999);
            slope.y = S * z * std::sqrt(1.0 + slope.x * slope.x);

            return slope;
        }
        break;
        default:
            return Vec2(0);
            break;
    }
}