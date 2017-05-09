#pragma once

#include <PhotonMath.h>
#include <Renderer.h>
#include <Spectral.h>
#include <Integrator.h>

namespace Photon {

    class PathTracer : public Integrator {
    public:
        PathTracer(const Scene& scene, uint32 spp = 64)
            : Integrator(scene, spp), _maxDepth(16), _useAdaptive(false),
            _adaptWidth(20), _adaptHeight(20), _spp(spp) {}

        PathTracer(const Scene& scene, RendererSettings settings)
            : Integrator(scene) {}
        
        void initialize();
        void startRender(EndCallback endCallback = EndCallback());

    private:
        Color estimateDirect(const SurfaceEvent& evt, Sampler& sampler) const;
        void renderTile(uint32 tId, uint32 tileId) const;
        void renderTileAdaptive(uint32 tId, uint32 tileId) const;

        Color tracePath(const Ray& ray, Sampler& sampler, const Point2ui& pixel = Point2ui(0)) const;

        Color subdivide(Sampler& sampler, const Point2& min, const Point2& max, 
                        std::vector<Color>& table, const Point2ui& pixel, Float weight) const;

        bool checkAdaptiveThreshold(const Color* samples, uint32 num) const;

        uint32 _spp;
        uint32 _maxDepth;
        uint32 _adaptWidth;
        uint32 _adaptHeight;
        bool _useAdaptive;
    };

}