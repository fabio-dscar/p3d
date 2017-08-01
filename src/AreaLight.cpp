#include <AreaLight.h>

#include <Ray.h>
#include <Shape.h>
#include <Sampling.h>

using namespace Photon;

std::shared_ptr<Shape> AreaLight::shape() const {
    return _shape;
}

bool AreaLight::isDelta() const {
    return false;
}

uint32 AreaLight::numSamples() const {
    return _numSamples;
}

Color AreaLight::L(const RayEvent& evt, const Vec3& w) const {
    if (Math::dot(evt.normal, w) > 0)
        return _Le;

    return Color::BLACK;
}

Float AreaLight::area() const {
    return _shape->area();
}

Color AreaLight::power() const {
    return _shape->area() * PI * _Le;
}

Color AreaLight::evalL(const SurfaceEvent& it, const Vec3& wo) const {
    // Only emit towards orientation
    if (dot(it.normal, wo) <= 0)
        return Color::BLACK;

    return _Le;
}

Color AreaLight::evalL(const PositionSample& sample, const Vec3& wo) const {
    // Only emit towards orientation
    if (dot(sample.frame.normal(), wo) <= 0)
        return Color::BLACK;

    return _Le;
}

Color AreaLight::samplePosition(const Point2& rand, PositionSample* sample) const {
    _shape->samplePosition(rand, sample);

    return power();
}

Float AreaLight::pdfPosition(const PositionSample& sample) const {
    return _shape->pdfPosition(sample);
}

Color AreaLight::sampleDirect(const Point2& rand, DirectSample* sample) const {
    _shape->sampleDirect(rand, sample);

    // Check if reference point normal and surface normal 
    // are oriented towards each other
    if (sample->pdf != 0 && dot(sample->wi, sample->ref->normal) >= 0 && dot(sample->normal, sample->wi) < 0)
        return _Le / sample->pdf;

    sample->pdf = 0;

    return Color::BLACK;
}

Float AreaLight::pdfDirect(const DirectSample& sample) const {
    // Reference normal and wi are on the same hemisphere and
    // ...
    if (dot(sample.wi, sample.ref->normal) >= 0 && dot(sample.normal, sample.wi) < 0)
        return _shape->pdfDirect(sample);

    return 0;
}

Color AreaLight::sampleEmitDirection(const Point2& rand, const PositionSample& pos, DirectionSample* sample) const {
    const Vec3 w = sampleCosHemisphere(rand);

    // Convert to world outgoing direction
    sample->wo  = pos.frame.toWorld(w);
    sample->pdf = pdfCosHemisphere(Frame::cosTheta(w));

    //return _Le;
    return power();
}

Float AreaLight::pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const {
    Float cosTheta = dot(sample.wo, pos.frame.normal());

    return INVPI * std::max((Float)0, cosTheta);
}