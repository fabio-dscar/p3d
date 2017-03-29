#pragma once

#include <Scene.h>

namespace Photon {

    class Scene;
    class WhittedRayTracer;

    struct RendererSettings {
        bool renderToScreen;
        bool exportFile;
        std::string outFileName;
        std::string outFormat;
    };

    class Renderer {
    public:
        Renderer() { initDefaultSettings(); }
        Renderer(const RendererSettings& settings) : _settings(settings) { }
        Renderer(const std::string& settingsFilePath) {
            initDefaultSettings();
            loadSettingsFile(settingsFilePath);
        }

        void initialize();
        void renderScene(const std::shared_ptr<Scene>& scene);
        bool hasCompleted();
        void waitForCompletion();
        const RendererSettings& settings();
        void exportImage();

    private:    
        void initDefaultSettings();
        void loadSettingsFile(const std::string& settingsFilePath);

        std::shared_ptr<Scene> _scene;
        std::shared_ptr<WhittedRayTracer> _raytracer;
        RendererSettings _settings;
    };

}