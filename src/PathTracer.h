#pragma once

#include <PhotonMath.h>
#include <Renderer.h>
#include <Spectral.h>
#include <Random.h>
#include <Integrator.h>

namespace Photon {

    class PathTracer : public Integrator {
    public:
        PathTracer(const Scene& scene, uint32 spp = 1)
            : Integrator(scene), _maxDepth(10), _random(), _spp(spp) {}

        PathTracer(const Scene& scene, RendererSettings settings)
            : Integrator(scene) {}
        
        void startRender(EndCallback endCallback = EndCallback());

    private:
        Color estimateDirect(const SurfaceEvent& evt) const;
        void renderTile(uint32 tId, uint32 tileId) const;

        Color tracePath(const Ray& ray, Float ior, uint32 depth, const Vec2ui& pixel = Vec2ui(0)) const;

        uint32 _spp;
        uint32 _maxDepth;
        RandGen _random;
    };

}