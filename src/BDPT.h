#pragma once

#include <Integrator.h>
#include <Ray.h>

namespace Photon {

    enum VertexType {
        SURFACE, CAMERA, LIGHT
    };

    struct PathVertex {
        Color beta;  // Throughput
        VertexType type;
        
        RayEvent evt;


    };

    enum PathType {
        CAMERA, LIGHT
    };

    class Path {
    public:
        PathType type;
        uint32 depth;
        std::vector<PathVertex> verts;

        
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