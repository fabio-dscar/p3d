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
            : Integrator(scene), _maxDepth(16), _random(), _useAdaptive(false),
            _adaptWidth(16), _adaptHeight(16), _spp(spp) {}

        PathTracer(const Scene& scene, RendererSettings settings)
            : Integrator(scene) {}
        
        void startRender(EndCallback endCallback = EndCallback());

    private:
        Color estimateDirect(const SurfaceEvent& evt) const;
        void renderTile(uint32 tId, uint32 tileId) const;
        void renderTileAdaptive(uint32 tId, uint32 tileId) const;

        Color tracePath(const Ray& ray, Float ior, uint32 depth, const Vec2ui& pixel = Vec2ui(0)) const;

        Color subdivide(const Point2& min, const Point2& max, std::vector<Color>& table, 
                        const Vec2ui& pixel, Float weight) const;

        bool checkAdaptiveThreshold(const Color* samples, uint32 num) const;

        uint32 _spp;
        uint32 _maxDepth;
        RandGen _random;
        bool _useAdaptive;
        uint32 _adaptWidth;
        uint32 _adaptHeight;
    };

}