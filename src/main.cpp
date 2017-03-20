#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <memory>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <Utils.h>
#include <Scene.h>
#include <NFFParser.h>
#include <Renderer.h>
#include <OpenGLRenderer.h>

using namespace Photon;
using namespace Photon::OpenGL;

std::shared_ptr<Renderer> _renderer = nullptr;
std::shared_ptr<OpenGLRenderer> _openglRenderer = nullptr;

std::shared_ptr<Scene> _scene = nullptr;

void clean() {
    _openglRenderer->cleanup();
}

void reshape(int w, int h) {
    _openglRenderer->resize(w, h);
}

void renderOpenGL() {
    _openglRenderer->renderToScreen();
}

int main(int argc, char* argv[]) {
    std::string filePath("balls_medium.nff");

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

    printf("resx = %d  resy= %d.\n", _scene->getCamera().getWidth(), 
                                     _scene->getCamera().getHeight());

    // Initialize OpenGL renderer
    _openglRenderer = std::make_shared<OpenGLRenderer>(_scene);
    _openglRenderer->initialize(argc, argv);
    _openglRenderer->setDisplayCallback(renderOpenGL);
    _openglRenderer->setReshapeCallback(reshape);
    _openglRenderer->setCleanupCallback(clean);

    // Initialize scene renderer and start rendering process
    _renderer = std::make_shared<Renderer>();
    _renderer->initialize();
    _renderer->renderScene(*_scene);

    // Start rendering to screen
    _openglRenderer->startRender();

    exit(EXIT_SUCCESS);
}
///////////////////////////////////////////////////////////////////////