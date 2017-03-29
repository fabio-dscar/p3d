#include <Scene.h>

#include <Camera.h>
#include <PointLight.h>
#include <Geometry.h>
#include <Bounds.h>
#include <UniformGrid.h>

#include <PhotonTracer.h>

using namespace Photon;

Scene::Scene() : _background(0.0f), _camera(), _lights(), _bounds(), _uniformGrid(nullptr) { }

void Scene::prepareRender(bool useGrid) {
    if (useGrid) {
        _uniformGrid = std::make_unique<UniformGrid>(*this, 2.0);
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

void Scene::addGeometry(const std::shared_ptr<Geometry> object) {
    // The scene incrementally builds its bounding box
    if (object->bounds().bounded()) {
        if (!_bounds.bounded() || _objects.size() == 0)
            _bounds = object->bounds();
        else
            _bounds = expand(_bounds, object->bounds());
    }

    _objects.push_back(object);
}

const std::vector<std::shared_ptr<Geometry>>& Scene::getGeometry() const {
    return _objects;
}

bool Scene::intersectRay(const Ray& ray, SurfaceEvent* info) const {
    if (_uniformGrid) {
        _uniformGrid->intersectRay(ray, info);
        return info->hit();
    } else {
        for (const std::shared_ptr<Geometry> obj : _objects)
            obj->intersectRay(ray, info);

        return info->hit();
    }
}

bool Scene::isOccluded(const Ray& ray) const {
    if (_uniformGrid) {
        return _uniformGrid->isOccluded(ray);
    } else {
        for (const std::shared_ptr<Geometry> obj : _objects)
            if (obj->isOccluded(ray))
                return true;
    }

    return false;
}

Bounds3 Scene::bounds() const {
    return _bounds;
}
