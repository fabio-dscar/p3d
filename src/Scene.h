#pragma once

#include <vector>
#include <memory>

#include <Vector.h>
#include <Camera.h>
#include <Bounds.h>
#include <Geometry.h>
#include <UniformGrid.h>

namespace Photon {

    // Forward declarations
    class Light;

    class Scene : public Geometry {
    public:
        Scene();

        void prepareRender(bool useGrid);

        void setBackgroundColor(const Color3& color);
        const Color3& getBackgroundColor() const;

        void addCamera(const Camera& camera);
        const Camera& getCamera() const;

        void addLight(Light const* light);
        const std::vector<Light const*>& getLights() const;

        void addGeometry(const std::shared_ptr<Geometry> object);
        const std::vector<std::shared_ptr<Geometry>>& getGeometry() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* info) const;
        bool isOccluded(const Ray& ray) const;

        Bounds3 bounds() const;
    private:
        Color3 _background;
        Camera _camera;
        Bounds3 _bounds;
        std::vector<Light const*> _lights;
        std::vector<std::shared_ptr<Geometry>> _objects;
        std::unique_ptr<UniformGrid> _uniformGrid;
    };

}