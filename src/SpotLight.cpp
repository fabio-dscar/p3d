#include <SpotLight.h>

using namespace Photon;

bool SpotLight::isDelta() const {
    return true;
}

Float SpotLight::calcFalloff(const Vec3& w) const {
    Vec3 wl = normalize(w);
    Float cosTheta = Frame::cosTheta(wl);

    if (cosTheta < _cosMax)
        return 0;

    if (cosTheta > _attStart) 
        return 1.0;

    Float delta = (cosTheta - _cosMax) / (_attStart - _cosMax);
    return (delta * delta) * (delta * delta);
}

Color SpotLight::power() const {
    return 2 * PI * _Le * (1.0 - 0.5 * (_attStart + _cosMax));
}

Color SpotLight::evalL(const SurfaceEvent& it, const Vec3& wo) const {
    return Color::BLACK;
}

Color SpotLight::evalL(const PositionSample& sample, const Vec3& wo) const {
    return Color::BLACK;
}

Color SpotLight::samplePosition(const Point2& rand, PositionSample* sample) const {
    sample->pos = _pos;
    sample->pdf = 1.0;

    return (4 * PI) * _Le;
}

Float SpotLight::pdfPosition(const PositionSample& sample) const {
    return 1.0;
}

Color SpotLight::sampleDirect(const Point2& rand, DirectSample* sample) const {
    Vec3 refToPos = _pos - sample->ref->point;

    sample->wi   = normalize(refToPos);
    sample->dist = refToPos.length();
    sample->pdf  = 1.0;
    sample->normal = Normal(-sample->wi);

    Float invDist = 1.0 / sample->dist;

    return (invDist * invDist) * _Le;
}

Float SpotLight::pdfDirect(const DirectSample& sample) const {
    return 1.0;
}

Color SpotLight::sampleEmitDirection(const Point2& rand, const PositionSample& pos, DirectionSample* sample) const {
    Vec3 w = sampleUniformCone(rand, _cosMax);
    
    sample->wo  = pos.frame.toWorld(w);
    sample->pdf = pdfUniformCone(_cosMax);

    return _Le * calcFalloff(w);
}

Float SpotLight::pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const {
    return pdfUniformCone(_cosMax);
}