#pragma once

#include <functional>
#include <vector>

#include <PhotonMath.h>
#include <Threading.h>
#include <Renderer.h>
#include <Random.h>
#include <Spectral.h>

#include <Integrator.h>

using namespace Photon::Threading;

namespace Photon {

    // Forward declaration
    class Scene;

    static const uint32 MAX_DEPTH = 6;

    class WhittedRayTracer : public Integrator {
    public:
        WhittedRayTracer(const Scene& scene, uint32 spp = 1)
            : Integrator(scene), _maxDepth(MAX_DEPTH), _random(), _spp(spp) { }
        
        WhittedRayTracer(const Scene& scene, RendererSettings settings)
            : Integrator(scene) { }

        void startRender(EndCallback endCallback = EndCallback());

    private:
        Color estimateDirect(const SurfaceEvent& evt) const;

        void renderTile(uint32 tId, uint32 tileId) const;

        // Whitted algorithm
        Color traceRay(const Ray& ray, Float ior, uint32 depth, const Vec2ui& pixel = Vec2ui(0)) const;

        uint32 _spp;
        uint32 _maxDepth;
        RandGen _random;
    };
    
}