#pragma once

#include <Integrator.h>

namespace Photon {

    enum VertexType {
        SURFACE, CAMERA, LIGHT
    };

    struct PathVertex {
        Color beta;  // Throughput
        VertexType type;
        

    };


    class BidirPathTracer : public Integrator {
    public:
        BidirPathTracer(const Scene& scene, uint32 spp = 1)
            : Integrator(scene) {}

        void startRender(EndCallback endCallback = EndCallback()) {

        }

    private:

    };

}