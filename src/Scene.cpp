#include <Scene.h>

using namespace Photon;

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

void Scene::addLight(const std::shared_ptr<Light>& light) {
    _lights.push_back(light);
}

const std::vector<std::shared_ptr<Light>>& Scene::getLights() const {
    return _lights;
}

void Scene::addGeometry(const std::shared_ptr<Geometry> object) {
    _objects.push_back(object);
}

const std::vector<std::shared_ptr<Geometry>>& Scene::getGeometry() const {
    return _objects;
}

bool Scene::intersectRay(const Ray& ray, SurfaceEvent* info) const {
    for (const std::shared_ptr<Geometry> obj : _objects)
        obj->intersectRay(ray, info);

    return info->hit();
}

bool Scene::isOccluded(const Ray& ray) const {
    for (const std::shared_ptr<Geometry> obj : _objects)
        if (obj->isOccluded(ray))
            return true;

    return false;
}