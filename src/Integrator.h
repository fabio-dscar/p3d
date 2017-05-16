#pragma once

#include <functional>

#include <PhotonMath.h>
#include <Vector.h>
#include <Threading.h>
#include <Spectral.h>
#include <Sampler.h>
#include <StratifiedSampler.h>
#include <RandomSampler.h>

using namespace Photon::Threading;

namespace Photon {

    // Forward declaration
    class Scene;
    class Light;
    class SurfaceEvent;

    typedef std::function<void()> EndCallback;

    static const uint32 TILE_SIZE = 64;

    struct ImageTile {
        uint32 x, y, w, h;
        std::unique_ptr<Sampler> samp;

        ImageTile() {}
        ImageTile(const Sampler& sampler, const Point2ui& tileXY, const Vec2ui& tileSizes) // uint32 tileX, uint32 tileY, uint32 tileWidth, uint32 tileHeight)
            : x(tileXY.x), y(tileXY.y), w(tileSizes.x), h(tileSizes.y) {
        
            samp = sampler.copy(y + h * x);
        }
    };

    class Integrator {
    public:
        Integrator(const Scene& scene) 
            : _scene(&scene), _tileSize(TILE_SIZE), _renderTask(nullptr), _tiles() { 

            _sampler = std::make_unique<StratifiedSampler>(16, 16, 16);
        }

        Integrator(const Scene& scene, uint32 spp)
            : _scene(&scene), _tileSize(TILE_SIZE), _renderTask(nullptr), _tiles() {

            _sampler = std::make_unique<StratifiedSampler>(16, 16, 16);
        }

        virtual void initialize();

        virtual void startRender(EndCallback endCallback = EndCallback()) = 0;

        virtual bool hasCompleted() const;
        virtual void waitForCompletion();
        virtual void cleanup();

    protected:
        Color sampleLight(const Light& light, const SurfaceEvent& evt, const Point2& randLight, const Point2& randBsdf) const;
        Color sampleBsdf(const SurfaceEvent& evt, const Point2& randBsdf) const;

        Scene const* _scene;
        uint32 _tileSize;
        std::vector<ImageTile> _tiles;
        std::shared_ptr<Task> _renderTask;
        std::unique_ptr<Sampler> _sampler;
    };

}