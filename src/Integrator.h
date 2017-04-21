#pragma once

#include <functional>

#include <PhotonMath.h>
#include <Vector.h>
#include <Threading.h>
#include <Spectral.h>

using namespace Photon::Threading;

namespace Photon {

    // Forward declaration
    class Scene;
    class Light;
    class SurfaceEvent;

    typedef std::function<void()> EndCallback;

    static const uint32 TILE_SIZE = 32;

    struct ImageTile {
        uint32 x, y, w, h;

        ImageTile() {}
        ImageTile(uint32 tileX, uint32 tileY, uint32 tileWidth, uint32 tileHeight)
            : x(tileX), y(tileY), w(tileWidth), h(tileHeight) {}
    };

    class Integrator {
    public:
        Integrator(const Scene& scene) 
            : _scene(&scene), _tileSize(TILE_SIZE), _renderTask(nullptr), _tiles() { }

        virtual void initialize();

        virtual void startRender(EndCallback endCallback = EndCallback()) = 0;

        virtual bool hasCompleted() const;
        virtual void waitForCompletion();
        virtual void cleanup();

    protected:
        Color sampleLight(const Light& light, const SurfaceEvent& evt, const Point2& randLight, const Point2& randBsdf) const;

        Scene const* _scene;
        uint32 _tileSize;
        std::vector<ImageTile> _tiles;
        std::shared_ptr<Task> _renderTask;
    };

}