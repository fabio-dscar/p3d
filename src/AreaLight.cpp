#include <AreaLight.h>

#include <Ray.h>
#include <Shape.h>

using namespace Photon;

Color3 AreaLight::L(const RayEvent& evt, const Vec3& w) const {
    if (Math::dot(evt.normal(), w) > 0)
        return _Le;

    return Color3(0);
}

Float AreaLight::area() const {
    return _shape->area();
}

void AreaLight::sampleLi(const RayEvent& ref, const Point2& rand, Point3* pos, Float* pdf) const {
    *pos = _shape->samplePosition(ref, rand, pdf).point();
}