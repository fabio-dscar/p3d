#pragma once

#include <vector>
#include <memory>

#include <Vector.h>
#include <Camera.h>
#include <PointLight.h>
#include <Geometry.h>

namespace Photon {

    class Scene : public Geometry {
    public:
        Scene() : _background(0.0f), _camera(), _lights() { }

        void setBackgroundColor(const Color3& color);
        const Color3& getBackgroundColor() const;

        void addCamera(const Camera& camera);
        const Camera& getCamera() const;

        void addLight(const std::shared_ptr<Light>& light);
        const std::vector<std::shared_ptr<Light>>& getLights() const;

        void addGeometry(const std::shared_ptr<Geometry> object);
        const std::vector<std::shared_ptr<Geometry>>& getGeometry() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* info) const;
        bool isOccluded(const Ray& ray) const;
    private:
        Color3 _background;
        Camera _camera;
        std::vector<std::shared_ptr<Light>> _lights;
        std::vector<std::shared_ptr<Geometry>> _objects;
    };

}