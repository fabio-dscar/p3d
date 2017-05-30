#include <Renderer.h>

#include <fstream>

#include <Vector.h>
#include <Scene.h>
#include <Camera.h>

#include <Integrator.h>
#include <WhittedRayTracer.h>
#include <PathTracer.h>
#include <BDPT.h>

#include <json\json.hpp>
#include <FreeImage.h>

using namespace Photon;

using json = nlohmann::json;

void Renderer::initialize() {
    
}

void Renderer::renderScene(const std::shared_ptr<Scene>& scene) {
    _scene = scene;
    _integrator = std::make_shared<PathTracer>(*scene); // WhittedRayTracer
    
    // Export file as an end callback after rendering
    std::function<void()> endCallback = EndCallback();
    if (_settings.exportFile)
        endCallback = [this]() { this->exportImage(); };

    // Init and start render
    _integrator->initialize();
    _integrator->startRender(endCallback);
}

void Renderer::waitForCompletion() {
    _integrator->waitForCompletion();
}

const RendererSettings& Renderer::settings() {
    return _settings;
}

bool Renderer::hasCompleted() {
    return _integrator->hasCompleted();
}

void Renderer::exportImage() {
    _scene->getCamera().film().exportImage(BufferType::COLOR);
}

void Renderer::initDefaultSettings() {
    // Init settings with default values
    _settings.renderToScreen = true;
    _settings.exportFile  = false;
    _settings.outFileName = "out";
    _settings.outFormat   = "tiff";
}

void Renderer::loadSettingsFile(const std::string& settingsFilePath) {
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