#pragma once

#include <Vector.h>
#include <Scene.h>
#include <WhittedRayTracer.h>

#include <json\json.hpp>
#include <FreeImage.h>

using json = nlohmann::json;

namespace Photon {

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

        void initialize() {
            
        }

        void renderScene(const std::shared_ptr<Scene>& scene) {
            _scene = scene;
            _raytracer = std::make_shared<WhittedRayTracer>(MAX_DEPTH, *scene);

            // Export file as an end callback after rendering
            std::function<void()> endCallback = EndCallback();
            if (_settings.exportFile)
                endCallback = [this]() { this->exportImage(); };

            // Init and start raytracer
            _raytracer->initialize();
            _raytracer->startRender(endCallback);
        }

        void waitForCompletion() {
            _raytracer->waitForCompletion();
        }

        const RendererSettings& settings() {
            return _settings;
        }

        bool hasCompleted() {
            return _raytracer->hasCompleted();
        }

        void exportImage() {
            FreeImage_Initialise();
            //FIBITMAP* bitmap = FreeImage_AllocateT(FIT_RGB, _scene->getCamera().getWidth(), _scene->getCamera().getHeight());
            FIBITMAP* bitmap = FreeImage_Allocate(_scene->getCamera().getWidth(), _scene->getCamera().getHeight(), 24);
            if (bitmap) {
                // Convert camera's film image to [0, 255 range]
                parallelFor(0, _scene->getCamera().getWidth(), 32, [&](uint32 i) {
                    for (uint32 y = 0; y < _scene->getCamera().getHeight(); y++) {
                        RGBQUAD color;
                        const Film& film = _scene->getCamera().film();

                        // Convert to [0, 255] range
                        Color3 pColor = film(i, y);
                        color.rgbRed = std::min(255u, (uint32)(pColor.r * 255.0f));
                        color.rgbGreen = std::min(255u, (uint32)(pColor.g * 255.0f));
                        color.rgbBlue = std::min(255u, (uint32)(pColor.b * 255.0f));

                        FreeImage_SetPixelColor(bitmap, i, y, &color);
                    }
                });

                std::string outName(_settings.outFileName + "." + _settings.outFormat);
                FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFormat(_settings.outFormat.c_str());
                if (format == FIF_UNKNOWN) {
                    std::cerr << "Error: Unknown export image format." << std::endl;
                    return;
                }

                FreeImage_Save(format, bitmap, outName.c_str(), 0);
                FreeImage_Unload(bitmap);
            }
        }

    private:    

        void initDefaultSettings() {
            // Init settings with default values
            _settings.renderToScreen = false;
            _settings.exportFile = true;
            _settings.outFileName = "out";
            _settings.outFormat = "tiff";
        }

        void loadSettingsFile(const std::string& settingsFilePath) {
            std::ifstream i(settingsFilePath);
            json settings;
            i >> settings;

            RendererSettings tmpSettings;
            try {
                tmpSettings = {
                    settings["renderToScreen"].get<bool>(),
                    settings["exportFile"].get<bool>(),
                    settings["exportFilename"].get<std::string>(),
                    settings["exportFormat"].get<std::string>()
                };

                _settings = tmpSettings;
            } catch (std::domain_error exception) {
                std::cerr << "[ERROR] Invalid settings.json file." << std::endl;
                std::cerr << exception.what() << std::endl;
                std::cerr << "Using default settings instead." << std::endl << std::endl;
            }     
        }

        std::shared_ptr<Scene> _scene;
        std::shared_ptr<WhittedRayTracer> _raytracer;
        RendererSettings _settings;
    };

}