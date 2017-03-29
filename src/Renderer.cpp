#include <Renderer.h>

#include <fstream>

#include <Vector.h>
#include <Scene.h>
#include <Camera.h>
#include <WhittedRayTracer.h>

#include <json\json.hpp>
#include <FreeImage.h>

using namespace Photon;

using json = nlohmann::json;

void Renderer::initialize() {

}

void Renderer::renderScene(const std::shared_ptr<Scene>& scene) {
    _scene = scene;
    _raytracer = std::make_shared<WhittedRayTracer>(*scene, MAX_DEPTH);

    // Export file as an end callback after rendering
    std::function<void()> endCallback = EndCallback();
    if (_settings.exportFile)
        endCallback = [this]() { this->exportImage(); };

    // Init and start raytracer
    _raytracer->initialize();
    _raytracer->startRender(endCallback);
}

void Renderer::waitForCompletion() {
    _raytracer->waitForCompletion();
}

const RendererSettings& Renderer::settings() {
    return _settings;
}

bool Renderer::hasCompleted() {
    return _raytracer->hasCompleted();
}

void Renderer::exportImage() {
    FreeImage_Initialise();
    //FIBITMAP* bitmap = FreeImage_AllocateT(FIT_RGB, _scene->getCamera().getWidth(), _scene->getCamera().getHeight());
    FIBITMAP* bitmap = FreeImage_Allocate(_scene->getCamera().width(), _scene->getCamera().height(), 24);
    if (bitmap) {
        // Convert camera's film image to [0, 255 range]
        parallelFor(0, _scene->getCamera().width(), 32, [&](uint32 i) {
            for (uint32 y = 0; y < _scene->getCamera().height(); y++) {
                RGBQUAD color;
                const Film& film = _scene->getCamera().film();

                // Convert to [0, 255] range
                Color3 pColor = film(i, y);
                color.rgbRed = Math::clamp<uint32>(pColor.x * 255.0, 0u, 255u);
                color.rgbGreen = Math::clamp<uint32>(pColor.y * 255.0, 0u, 255u);
                color.rgbBlue = Math::clamp<uint32>(pColor.z * 255.0, 0u, 255u);

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

void Renderer::initDefaultSettings() {
    // Init settings with default values
    _settings.renderToScreen = true;
    _settings.exportFile = false;
    _settings.outFileName = "out";
    _settings.outFormat = "tiff";
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