#include <Scene.h>

#include <Camera.h>
#include <PointLight.h>
#include <Shape.h>
#include <Bounds.h>
#include <UniformGrid.h>

#include <AreaLight.h>

#include <PhotonTracer.h>

using namespace Photon;

Scene::Scene() : _background(0), _camera(), _lights(), _bounds(), 
                 _uniformGrid(nullptr), _hideLights(false), _useGrid(true), 
                 _lightDistr(nullptr), _lightStrat(POWER) { }

void Scene::prepareRender() {
    // Initialize spatial subdivision, if needed
    if (_useGrid) {
        _uniformGrid = std::make_unique<UniformGrid>(*this, 2.0f);
        //_uniformGrid = std::make_unique<UniformGrid>(*this, Vec3ui(6, 6, 6));
        _uniformGrid->initialize();
    }

    // Initialize light distribution, use uniform if unspecified
    std::vector<Float> vals(_lights.size());

    for (uint32 l = 0; l < _lights.size(); ++l)
        vals[l] = (_lightStrat == POWER) ? _lights[l]->power().lum() : 1;

    _lightDistr = std::make_unique<DiscretePdf1D>(vals);
}

DiscretePdf1D* Scene::lightDistribution() const {
    return _lightDistr.get();
}

LightStrategy Scene::lightStrategy() const {
    return _lightStrat;
}

const Light* Scene::sampleLightPdf(Float rand, Float* lightPdf) const {
    if (!_lightDistr)
        return nullptr;

    uint32 lightIdx = _lightDistr->sample(rand);
    
    if (lightPdf)
        *lightPdf = _lightDistr->pdf(lightIdx);

    return _lights[lightIdx];
}

void Scene::setBackgroundColor(const Color& color) {
    _background = color;
}

const Color& Scene::getBackgroundColor() const {
    return _background;
}

void Scene::addCamera(const Camera& camera) {
    _camera = &camera;
}

const Camera& Scene::getCamera() const {
    return *_camera;
}

void Scene::addLight(Light const* light) {
    _lights.push_back(light);
}

const std::vector<Light const*>& Scene::getLights() const {
    return _lights;
}

void Scene::useGrid(bool state) {
    _useGrid = state;
}

void Scene::addShape(const std::shared_ptr<Shape> object) {
    // The scene incrementally builds its bounding box
    if (object->bbox().isBounded()) {
        if (!_bounds.isBounded() || _objects.size() == 0)
            _bounds = object->bbox();
        else
            _bounds.expand(object->bbox());
    }

    // If the shape emits light, add to the light list
    if (object->isLight()) {
        _lights.push_back(object->areaLight());

        if (!_hideLights)
            _objects.push_back(object);
    } else {
        _objects.push_back(object);
    }
}

const std::vector<std::shared_ptr<Shape>>& Scene::getShapes() const {
    return _objects;
}

bool Scene::intersectRay(const Ray& ray, SurfaceEvent* info) const {
    if (_uniformGrid) {
        // Use acceleration structure
        _uniformGrid->intersectRay(ray, info);

        // If there is a hit, compute surface intersection info
        if (info->hit())
            info->obj->computeSurfaceEvent(ray, *info);

        return info->hit();
    } else {
        for (const std::shared_ptr<Shape> obj : _objects)
            obj->intersectRay(ray, info);

        // If there is a hit, compute surface intersection info
        if (info->hit())
            info->obj->computeSurfaceEvent(ray, *info);

        return info->hit();
    }
}

bool Scene::isOccluded(const Ray& ray) const {
    if (_uniformGrid) {
        return _uniformGrid->isOccluded(ray);
    } else {
        for (const std::shared_ptr<Shape> obj : _objects)
            if (obj->isOccluded(ray))
                return true;
    }

    return false;
}

Bounds3 Scene::bounds() const {
    return _bounds;
}
