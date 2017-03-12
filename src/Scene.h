#pragma once

#include <vector>

#include <Vector.h>
#include <Camera.h>
#include <PointLight.h>

namespace Photon {

    class Scene {
    public:
        Scene() : _background(0.0f), _camera(), _lights() { }

        void setBackgroundColor(const Color3& color);
        const Color3& getBackgroundColor() const;

        void addCamera(const Camera& camera);
        const Camera& getCamera() const;

        void addLight(const PointLight& light);
        const std::vector<PointLight>& getLights() const;

    private:
        Color3 _background;
        Camera _camera;
        std::vector<PointLight> _lights;
    };

}