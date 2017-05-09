#include <PointLight.h>

using namespace Photon;

bool PointLight::isDelta() const {
    return true;
}

Color PointLight::evalL(const SurfaceEvent& it, const Vec3& wo) const {
    return Color::BLACK;
}

Color PointLight::evalL(const PositionSample& sample, const Vec3& wo) const {
    return Color::BLACK;
}

Color PointLight::samplePosition(const Point2& rand, PositionSample* sample) const {
    sample->pos = _pos;
    sample->pdf = 1.0;

    return (4 * PI) * _Le;
}

Float PointLight::pdfPosition(const PositionSample& sample) const {
    return 1.0;
}

Color PointLight::sampleDirect(const Point2& rand, DirectSample* sample) const {
    Vec3 refToPos = _pos - sample->ref->point;

    sample->wi   = normalize(refToPos);
    sample->dist = refToPos.length();
    sample->pdf  = 1.0;
    sample->normal = Normal(-sample->wi);

    Float invDist = 1.0 / sample->dist;

    return (invDist * invDist) * _Le;
}

Float PointLight::pdfDirect(const DirectSample& sample) const {
    return 1.0;
}

Color PointLight::sampleEmitDirection(const Point2& rand, const PositionSample& pos, DirectionSample* sample) const {
    sample->wo = sampleUniformSphere(rand).posVec();
    sample->pdf = INV4PI;

    return _Le;
}

Float PointLight::pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const {
    return INV4PI;
}