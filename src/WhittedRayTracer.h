#pragma once

#include <functional>
#include <vector>

#include <MathDefs.h>
#include <Threading.h>
#include <Renderer.h>
#include <Random.h>

using namespace Photon::Threading;

namespace Photon {

    // Forward declaration
    class Scene;

    static const uint32 MAX_DEPTH = 8;
    static const uint32 TILE_SIZE = 64;

    typedef std::function<void()> EndCallback;

    struct ImageTile {
        uint32 x, y, w, h;

        ImageTile() { }
        ImageTile(uint32 tileX, uint32 tileY, uint32 tileWidth, uint32 tileHeight) 
            : x(tileX), y(tileY), w(tileWidth), h(tileHeight) { }
    };

    class WhittedRayTracer {
    public:
        WhittedRayTracer(const Scene& scene, uint32 maxDepth, uint32 spp = 1)
            : _maxDepth(maxDepth), _scene(&scene), _renderTask(nullptr), _tiles(), _random(), _spp(spp) { }
        
        WhittedRayTracer(const Scene& scene, RendererSettings settings) { }

        void initialize();
        void startRender(EndCallback endCallback = EndCallback());

        bool hasCompleted() const;
        void waitForCompletion();
        void cleanup();

    private:
        void renderTile(uint32 tId, uint32 tileId) const;

        // Whitted algorithm
        Color3 traceRay(const Ray& ray, Float ior, uint32 depth, const Vec2ui& pixel = Vec2ui(0)) const;

        uint32 _spp;
        uint32 _maxDepth;
        Scene const* _scene;
        std::vector<ImageTile> _tiles;
        std::shared_ptr<Task> _renderTask;
        RandGen _random;
    };
    
}