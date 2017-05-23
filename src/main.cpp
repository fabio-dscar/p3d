#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <memory>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <json\json.hpp>
#include <FreeImage.h>

#include <PhotonTracer.h>
#include <Utils.h>
#include <Scene.h>
#include <NFFParser.h>
#include <Renderer.h>
#include <OpenGLRenderer.h>
#include <Threading.h>
#include <Timer.h>
#include <Resources.h>

using namespace Photon;
using namespace Photon::OpenGL;

using json = nlohmann::json;

std::shared_ptr<Renderer> _renderer = nullptr;
std::shared_ptr<OpenGLRenderer> _openglRenderer = nullptr;
std::shared_ptr<Scene> _scene = nullptr;

void finish() {
    std::cout << "Rendering has completed. Press any key to terminate." << std::endl;
    std::cin.get();
    exit(EXIT_SUCCESS);
}

void clean() {
    _openglRenderer->cleanup();
}

void reshape(int w, int h) {
    _openglRenderer->resize(w, h);
}

void renderOpenGL() {
    _openglRenderer->renderToScreen();

    if (_renderer->hasCompleted()) {
        _openglRenderer->renderToScreen(true);
        finish();
    }
}

void mousePress(int button, int state, int x, int y) {
    std::cout << "Click at pixel: (" << x << ", " << _scene->getCamera().height() - y << ")" << std::endl;
}

void photonInit() {
    // Initialize threading
    Threading::initThreads(Threading::getNumberOfProcessors());

    // Initialize resource manager
    Resources::initialize();

    // Initialize FreeImage
    FreeImage_Initialise();
}

void photonShutdown() {
    // Shutdown FreeImage
    FreeImage_DeInitialise();
}

#include <Frame.h>
#include <Sampling.h>
#include <Random.h>

#include <Records.h>

int main(int argc, char* argv[]) {
    std::string filePath("oren-box.nff");

    // Command line arguments
    if (argc < 1) {
        std::cerr << "Usage: " << argv[0] << " <NFF_file>" << std::endl;
        std::cin.get();
        return EXIT_FAILURE;
    } else if (argc > 1) {
        filePath = std::string(argv[1]);
    }

    // Parse scene
    _scene = Utils::NFFParser::fromFile(filePath);
    if (!_scene)
        Utils::throwError("Failed to load scene.");

    // Init system
    photonInit();

    // Prepare scene for rendering
    _scene->prepareRender();

    Utils::Timer t;

    // Initialize scene renderer and start rendering process
    _renderer = std::make_shared<Renderer>("settings.json");
    _renderer->initialize();
    _renderer->renderScene(_scene);
    
    if (_renderer->settings().renderToScreen) {
        // Initialize OpenGL renderer
        _openglRenderer = std::make_shared<OpenGLRenderer>(_scene);
        _openglRenderer->initialize(argc, argv);
        _openglRenderer->setDisplayCallback(renderOpenGL);
        _openglRenderer->setReshapeCallback(reshape);
        _openglRenderer->setCleanupCallback(clean);
        glutMouseFunc(mousePress);

        // Start rendering to screen
        _openglRenderer->startRender();
    } else {
        // Just wait for rendering to finish
        _renderer->waitForCompletion();
        t.stop();
        std::cout << t.elapsed() / 1000.0 << " s" << std::endl;
        finish();
    }

    exit(EXIT_SUCCESS);
}
///////////////////////////////////////////////////////////////////////