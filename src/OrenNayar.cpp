#include <OrenNayar.h>

using namespace Photon;

OrenNayar::OrenNayar(const Color& rho, Float sigma)
    : BSDF(BSDFType(BSDFType::DIFFUSE |
                    BSDFType::REFLECTION)) {

    _kd = std::make_shared<ConstTexture<Color>>(rho);
    _sigma = sigma;
}

OrenNayar::OrenNayar(const std::shared_ptr<Texture<Color>>& diffuseTex, Float sigma)
    : BSDF(BSDFType(BSDFType::DIFFUSE |
                    BSDFType::REFLECTION)) {

    _kd = diffuseTex;
    _sigma = sigma;
}

Color OrenNayar::eval(const BSDFSample& sample) const {
    if (!hasType(sample.type, _type)
        || Frame::cosTheta(sample.wi) <= 0
        || Frame::cosTheta(sample.evt->wo()) <= 0)
        return Color::BLACK;

    const Vec3 wi = sample.wi;
    const Vec3 wo = sample.evt->wo();

    Float sinThetaI = Frame::sinTheta(wi);
    Float sinThetaO = Frame::sinTheta(wo);

    // Compute cosine term of Oren-Nayar model
    Float maxCos = 0;
    if (sinThetaI > F_EPSILON && sinThetaO > F_EPSILON) {
        Float sinPhiI = Frame::sinPhi(wi);
        Float cosPhiI = Frame::cosPhi(wi);

        Float sinPhiO = Frame::sinPhi(wo);
        Float cosPhiO = Frame::cosPhi(wo);

        Float dCos = cosPhiI * cosPhiO + sinPhiI * sinPhiO;

        maxCos = std::max((Float)0.0, dCos);
    }

    const Float sigma2 = _sigma * _sigma;

    Float thetaI = Math::acosSafe(Frame::cosTheta(wi));
    Float thetaO = Math::acosSafe(Frame::cosTheta(wo));
    Float alpha  = std::max(thetaI, thetaO);
    Float beta   = std::min(thetaI, thetaO);

    Float sinAlpha, sinBeta, tanBeta;
    if (Frame::cosTheta(wi) > Frame::cosTheta(wo)) {
        sinAlpha = sinThetaO;
        sinBeta  = sinThetaI;
        tanBeta  = sinThetaI / Frame::cosTheta(wi);
    } else {
        sinAlpha = sinThetaI;
        sinBeta  = sinThetaO;
        tanBeta  = sinThetaO / Frame::cosTheta(wo);
    }

    Float tmp1 = sigma2 / (sigma2 + 0.09);
    Float tmp2 = (4 * INVPI * INVPI) * alpha * beta;
    Float tmp3 = 2 * beta * INVPI;

    Float C1 = 1.0 - 0.5 * sigma2 / (sigma2 + 0.33);
    Float C2 = 0.45 * tmp1;
    Float C3 = 0.125 * tmp1 * tmp2 * tmp2;
    Float C4 = 0.17 * sigma2 / (sigma2 + 0.13);

    if (maxCos > 0)
        C2 *= sinAlpha;
    else
        C2 *= sinAlpha - tmp3 * tmp3 * tmp3;

    /* Compute tan(0.5 * (alpha+beta)) using the half-angle formulae */
    Float tanHalf = (sinAlpha + sinBeta) / (Math::sqrtSafe(1.0 - sinAlpha * sinAlpha)
                                            + Math::sqrtSafe(1.0 - sinBeta  * sinBeta));

    Color rho = _kd->fetch(sample.evt->uv());

    Color snglScat = rho * (C1 + maxCos * C2 * tanBeta + (1.0 - std::abs(maxCos)) * C3 * tanHalf);
    Color dblScat  = rho * rho * (C4 * (1.0 - maxCos * tmp3 * tmp3));

    return  (snglScat + dblScat) * INVPI;
}