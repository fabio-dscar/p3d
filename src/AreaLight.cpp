#include <AreaLight.h>

#include <Ray.h>
#include <Shape.h>

using namespace Photon;

Color AreaLight::L(const RayEvent& evt, const Vec3& w) const {
    if (Math::dot(evt.normal, w) > 0)
        return _Le;

    return Color(0);
}

Float AreaLight::area() const {
    return _shape->area();
}

void AreaLight::sampleLi(const RayEvent& ref, const Point2& rand, Point3* pos, Float* pdf) const {
    PositionSample sample(ref);    

    _shape->samplePosition(rand, &sample);


}





Color AreaLight::evalL(const SurfaceEvent& it, const Vec3& wo) const {
    // Only emit towards orientation
    if (dot(it.sFrame.normal(), wo) <= 0)
        return Color::BLACK;

    return _Le;
}

Color AreaLight::samplePosition(const Point2& rand, PositionSample* sample) const {
    _shape->samplePosition(rand, sample);
    return _Le;
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
    Vec3 local = sampleCosHemisphere(rand);

    // Convert to world outgoing direction
    sample->wo = pos.frame.toWorld(local);
    sample->pdf = pdfCosHemisphere(Frame::cosTheta(local));

    return Color(1);
}

Float AreaLight::pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const {
    Float cosAng = dot(sample.wo, pos.frame.normal());
    return INVPI * std::max((Float)0.0, cosAng);
}