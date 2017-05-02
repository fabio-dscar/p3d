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
        _openglRenderer->renderToScreen();
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
}

void photonShutdown() {

}

#include <Frame.h>
#include <Sampling.h>
#include <Random.h>

struct Coisas {
    Vec3 dir;
    Float pdf;
};

#include <Quadric.h>
#include <OrenNayar.h>
#include <Lambertian.h>
#include <Specular.h>
#include <ThinSpecular.h>

int main(int argc, char* argv[]) {
    std::string filePath("oren-box.nff");
    
    /*std::vector<Coisas> vecs;
    RandGen rand;
    Frame frame = Frame(Normal(0, 0, -1));
    const uint32 ang = 1;
    
    for (uint32 i = 0; i < 48; ++i) {
        Coisas item;
        item.dir = sampleUniformSphericalCap(Point2(rand.uniformFloat(), rand.uniformFloat()),
                                             std::cos(Math::radians(ang)));
        item.dir = frame.toWorld(item.dir);
        item.pdf = pdfUniformSphericalCap(std::cos(Math::radians(ang)));
        vecs.push_back(item);

    }

    return 0;*/

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

    //std::shared_ptr<Shape> quadric;

    //quadric = std::make_shared<Quadric>(-1, -1, 1, 0, 0, 0, 0, 0, 0, -0.05);
    //quadric = std::make_shared<Quadric>(2, 2, -1, 0, 0, 0, 0, 0, 0, -0.10);
    //quadric = std::make_shared<Quadric>(1, 1, 0, 0, 0, 0, 0, 0, 0, -0.15);
    //quadric->setBsdf(new Lambertian(Color(0.6, 0.6, 0.6)));
    //quadric->setBsdf(new Specular(1.5, 1.0, Color(0), Color(1)));

    //_scene->addShape(quadric);

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