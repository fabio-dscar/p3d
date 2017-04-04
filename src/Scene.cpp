#include <Scene.h>

#include <Camera.h>
#include <PointLight.h>
#include <Shape.h>
#include <Bounds.h>
#include <UniformGrid.h>

#include <AreaLight.h>

#include <PhotonTracer.h>

using namespace Photon;

Scene::Scene() : _background(0.0f), _camera(), _lights(), _bounds(), _uniformGrid(nullptr) { }

void Scene::prepareRender(bool useGrid) {
    if (useGrid) {
        _uniformGrid = std::make_unique<UniformGrid>(*this, (Float)2.0);
        _uniformGrid->initialize();
    }
}

void Scene::setBackgroundColor(const Color3& color) {
    _background = color;
}

const Color3& Scene::getBackgroundColor() const {
    return _background;
}

void Scene::addCamera(const Camera& camera) {
    _camera = camera;
}

const Camera& Scene::getCamera() const {
    return _camera;
}

void Scene::addLight(Light const* light) {
    _lights.push_back(light);
}

const std::vector<Light const*>& Scene::getLights() const {
    return _lights;
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
    if (object->isLight())
        _lights.push_back(object->areaLight());
    else 
        _objects.push_back(object);
}

const std::vector<std::shared_ptr<Shape>>& Scene::getShapes() const {
    return _objects;
}

bool Scene::intersectRay(const Ray& ray, SurfaceEvent* info) const {
    if (_uniformGrid) {
        _uniformGrid->intersectRay(ray, info);
        return info->hit();
    } else {
        for (const std::shared_ptr<Shape> obj : _objects)
            obj->intersectRay(ray, info);

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
