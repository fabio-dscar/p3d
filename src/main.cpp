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

#include <json\json.hpp>

#include <FreeImage.h>

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
        _openglRenderer->renderToScreen();
        finish();
    }
}

int main(int argc, char* argv[]) {
    std::string filePath("balls_low.nff");

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

        // Start rendering to screen
        _openglRenderer->startRender();
    } else {
        _renderer->waitForCompletion();
        finish();
    }
    
    /* else {
        _renderer->waitForCompletion();
        std::cout << "Completed!" << std::endl;
        
        FreeImage_Initialise();
        //FIBITMAP* bitmap = FreeImage_AllocateT(FIT_RGB, _scene->getCamera().getWidth(), _scene->getCamera().getHeight());
        FIBITMAP* bitmap = FreeImage_Allocate(_scene->getCamera().getWidth(), _scene->getCamera().getHeight(), 24);
        if (bitmap) {

            //
            parallelFor(0, _scene->getCamera().getWidth(), 32, [&](uint32 i) {
                for (uint32 y = 0; y < _scene->getCamera().getHeight(); y++) {
                    RGBQUAD color;
                    const Film& film = _scene->getCamera().film();

                    Color3 pColor = film(i, y);
                    color.rgbRed = std::min(255u, (uint32)(pColor.r * 255.0f));
                    color.rgbGreen = std::min(255u, (uint32)(pColor.g * 255.0f));
                    color.rgbBlue = std::min(255u, (uint32)(pColor.b * 255.0f));

                    FreeImage_SetPixelColor(bitmap, i, y, &color);
                }
            });

            //FreeImage_Save(FIF_BMP, bitmap, "test1.bmp", 0);
            FreeImage_Save(FIF_PNG, bitmap, "test.png", 0);

            FreeImage_Unload(bitmap);
        }

    }*/

    exit(EXIT_SUCCESS);
}
///////////////////////////////////////////////////////////////////////