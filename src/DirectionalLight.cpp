#include <DirectionalLight.h>

#include <Scene.h>
#include <Sphere.h>

using namespace Photon;

DirectionalLight::DirectionalLight() 
    : Light(Color(1)), _w(Vec3(0, 0, -1)), _sceneCenter(0, 0, 0), _sceneRadius(1) { }

DirectionalLight::DirectionalLight(const Transform& objToWorld, const Color& color)
    : Light(color, objToWorld), _w(normalize(objToWorld(Vec3(0, 0, -1)))), _sceneCenter(0, 0, 0), _sceneRadius(1) { }

void DirectionalLight::initialize(const Scene& scene) {
    const Bounds3 sceneBbox = scene.bounds();
    const Sphere s = sceneBbox.sphere();
    
    _sceneCenter = s.pos();
    _sceneRadius = s.radius();
}

bool DirectionalLight::isDelta() const {
    return true;
}

Color DirectionalLight::power() const {
    return _Le * PI * _sceneRadius * _sceneRadius;
}

// Evaluate L for outgoing wo at intersection
Color DirectionalLight::evalL(const SurfaceEvent& it, const Vec3& wo) const {
    return Color::BLACK;
}

Color DirectionalLight::evalL(const PositionSample& sample, const Vec3& wo) const {
    return Color::BLACK;
}

// Sample a position on the surface
Color DirectionalLight::samplePosition(const Point2& rand, PositionSample* sample) const {
    Frame frame = Frame(Normal(_w));

    Point2 randDisk = sampleConcentricDisk(rand);
    Vec3 ptDisk = randDisk.x * frame.x() + randDisk.y * frame.y();
    Point3 ptSceneDisk = _sceneCenter + _sceneRadius * ptDisk;

    sample->pos   = ptSceneDisk + _sceneRadius * -_w;
    sample->pdf   = 1.0 / (PI * _sceneRadius * _sceneRadius);
    sample->frame = frame;
    
    return _Le;
}

Float DirectionalLight::pdfPosition(const PositionSample& sample) const {
    return 1.0 / (PI * _sceneRadius * _sceneRadius);
}

// Sample an incoming direction to a distant reference position
Color DirectionalLight::sampleDirect(const Point2& rand, DirectSample* sample) const {
    const Point3 ref = sample->ref->point;

    sample->wi     = -_w;
    sample->pdf    = 1;
    sample->normal = Normal(_w);
    sample->dist   = dist(ref, ref + _w * (2 * _sceneRadius));

    return _Le;
}

Float DirectionalLight::pdfDirect(const DirectSample& sample) const {
    return 1.0;
}

// Sample an outgoing direction from a local reference position
Color DirectionalLight::sampleEmitDirection(const Point2& rand, const PositionSample& pos, DirectionSample* sample) const {
    sample->wo  = _w;
    sample->pdf = 1;

    return _Le;
}

Float DirectionalLight::pdfEmitDirection(const PositionSample& pos, const DirectionSample& sample) const {
    return 0;
}