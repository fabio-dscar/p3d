#include <Shape.h>

#include <Ray.h>
#include <Bounds.h>
#include <BSDF.h>

using namespace Photon;

Shape::Shape() : _light(nullptr), _bsdf(nullptr), _twoSided(false) {}
Shape::Shape(const Transform& objToWorld) : _objToWorld(objToWorld), _light(nullptr), _bsdf(nullptr), _twoSided(false) {
    _worldToObj = inverse(objToWorld);
}

Shape::Shape(const AreaLight* light) : _light(light), _bsdf(nullptr), _twoSided(false) {}

void Shape::setTransform(const Transform& objToWorld) {
    _objToWorld = objToWorld;
    _worldToObj = inverse(_objToWorld);
}

void Shape::setTransform(const Transform& objToWorld, const Transform& worldToObj) {
    _objToWorld = objToWorld;
    _worldToObj = worldToObj;
}

void Shape::setBsdf(const BSDF* bsdf) {
    _bsdf = bsdf;
}

const BSDF* Shape::bsdf() const {
    return _bsdf;
}

bool Shape::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    return false;
}

bool Shape::isOccluded(const Ray& ray) const {
    return false;
}

Bounds3 Shape::bbox() const {
    return Bounds3(Point3(0));
}

Float Shape::area() const {
    return 0;
}

bool Shape::isLight() const {
    return _light != nullptr;
}

void Shape::setLight(const AreaLight* light) {
    _light = light;
}

const AreaLight* Shape::areaLight() const {
    return _light;
}

void Shape::computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const {
    if (_twoSided) {
        evt.normal *= sign(Frame::cosTheta(evt.wo));
        evt.gFrame = Frame(evt.normal);
    }
}

void Shape::samplePosition(const Point2& rand, PositionSample* sample) const {
    sample->pos = Point3(0);
}

Float Shape::pdfPosition(const PositionSample& sample) const {
    return 0;
}

void Shape::sampleDirect(const Point2& rand, DirectSample* sample) const {
    sample->wi = Vec3(0);
}

Float Shape::pdfDirect(const DirectSample& sample) const {
    return 0;
}