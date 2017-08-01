#pragma once

#include <vector>
#include <memory>

#include <Vector.h>
#include <Camera.h>
#include <Bounds.h>
#include <Shape.h>
#include <UniformGrid.h>
#include <Distribution.h>

namespace Photon {

    // Forward declarations
    class Light;

    enum LightStrategy {
        ALL_LIGHTS = 0,
        UNIFORM = 1,
        POWER = 2
    };

    class Scene {
    public:
        Scene();

        void prepareRender();

        void setBackgroundColor(const Color& color);
        const Color& getBackgroundColor() const;

        void addCamera(const Camera& camera);
        const Camera& getCamera() const;

        void addLight(Light* light);
        void addAreaLight(AreaLight* light);
        const std::vector<Light*>& getLights() const;

        void addShape(const std::shared_ptr<Shape> object);
        const std::vector<std::shared_ptr<Shape>>& getShapes() const;

        bool intersectRay(const Ray& ray, SurfaceEvent* info) const;
        bool isOccluded(const Ray& ray) const;

        Bounds3 bounds() const;

        void useGrid(bool state);

        const Light* sampleLightPdf(Float rand, Float* lightPdf) const;
        LightStrategy lightStrategy() const;
        DiscretePdf1D* lightDistribution() const;
    private:
        Color _background;
        const Camera* _camera;
        Bounds3 _bounds;
        std::vector<Light*> _lights;
        std::vector<std::shared_ptr<Shape>> _objects;
        std::unique_ptr<UniformGrid> _uniformGrid;
        std::unique_ptr<DiscretePdf1D> _lightDistr;
        bool _hideLights;
        bool _useGrid;
        LightStrategy _lightStrat;
    };

}