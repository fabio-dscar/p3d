#pragma once

#include <PhotonMath.h>
#include <Renderer.h>
#include <Spectral.h>
#include <Integrator.h>

#include <deque>

namespace Photon {

    /*struct RRStats {
        uint32 numRays;
        uint32 spp;

        uint32 sumRays;
        Float  sumVals;
        Float  sumValsSqr;

        std::deque<Float> vals;
        std::deque<Float> valsSqr;
        std::deque<uint32> rays;

        RRStats() {

        }
        
        void reset() {
            numRays = 0;
        }

        Float threshold(uint32 nRays, Float val) {
            sumRays -= rays.back();
            sumVals -= vals.back();
            sumValsSqr -= valsSqr.back();

            rays.pop_back();
            vals.pop_back();
            valsSqr.pop_back();

            sumRays += nRays;
            sumVals += val;
            sumValsSqr += val * val;

            rays.push_front(nRays);
            vals.push_front(val);
            valsSqr.push_front(val * val);

            Float invSpp = 1.0 / spp;
            return std::sqrt((sumValsSqr - invSpp * sumVals * sumVals) / sumRays);
        }
    };*/

    class PathTracer : public Integrator {
    public:
        PathTracer(const Scene& scene, uint32 spp = 1024)
            : Integrator(scene, spp), _maxDepth(16), _useAdaptive(false),
            _adaptWidth(20), _adaptHeight(20), _spp(spp) {}

        PathTracer(const Scene& scene, RendererSettings settings)
            : Integrator(scene) {}
        
        void initialize();
        void startRender(EndCallback endCallback = EndCallback());

    private:
        Color estimateDirectAll(const SurfaceEvent& evt, Sampler& sampler) const;
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