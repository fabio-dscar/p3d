#pragma once

#include <Vector.h>
#include <Scene.h>
#include <WhittedRayTracer.h>

namespace Photon {

    class Renderer {
    public:
        Renderer() { }

        void initialize() {
            
        }

        void renderScene(Scene& scene) {
            _raytracer = std::make_shared<WhittedRayTracer>(MAX_DEPTH, scene);

            // Init and start raytracer
            _raytracer->initialize();
            _raytracer->startRender();
        }

    private:    
        std::shared_ptr<WhittedRayTracer> _raytracer;
    };

}