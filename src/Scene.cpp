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

void Scene::addLight(const PointLight& light) {
    _lights.push_back(light);
}

const std::vector<PointLight>& Scene::getLights() const {
    return _lights;
}