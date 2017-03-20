#pragma once

#include <functional>
#include <vector>

#include <MathDefs.h>
#include <Ray.h>
#include <Scene.h>
#include <Threading.h>

#include <glm/ext.hpp>

using namespace Photon::Threading;

namespace Photon {

    static const uint32 MAX_DEPTH = 6;
    static const uint32 TILE_SIZE = 128;

    struct ImageTile {
        uint32 x, y, w, h;

        ImageTile() { }
        ImageTile(uint32 tileX, uint32 tileY, uint32 tileWidth, uint32 tileHeight) 
            : x(tileX), y(tileY), w(tileWidth), h(tileHeight) { }
    };

    class WhittedRayTracer {
    public:
        WhittedRayTracer(uint32 maxDepth, Scene& scene) 
            : _maxDepth(maxDepth), _scene(&scene), _renderTask(nullptr), _tiles() { }

        void initialize();
        void startRender();

        void waitForCompletion();
        void cleanup();

    private:
        void renderTile(const Scene& scene, uint32 tId, uint32 tileId) const;

        // Whitted algorithm
        Color3 traceRay(const Scene& scene, const Ray& ray, float ior, unsigned int depth) const;

        uint32 _maxDepth;
        Scene* _scene;
        std::vector<ImageTile> _tiles;
        std::shared_ptr<Task> _renderTask;
    };
    
}