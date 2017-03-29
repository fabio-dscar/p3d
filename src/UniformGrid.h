#pragma once

#include <memory>

#include <Bounds.h>
#include <Geometry.h>

namespace Photon {

    // Forward declaration
    class Scene;

    typedef std::shared_ptr<Geometry> GridObject;

    struct Voxel {
        Voxel() : objIDs(0) { }

        void addObject(uint32 objID);
        bool hasObjects() const {
            return objIDs.size() > 0;
        }

        std::vector<uint32> objIDs;
    };

    class UniformGrid {
    public:
        UniformGrid(const Scene& scene, const Vec3ui& dims);
        UniformGrid(const Scene& scene, Float m = 1);

        void initialize();

        Voxel& voxel(uint32 x, uint32 y, uint32 z);
        const Voxel& voxel(uint32 x, uint32 y, uint32 z) const;
        const Voxel& voxel(const Point3& pos) const;
        const Voxel& voxel(const Point3ui& pt) const;
        Point3ui gridPos(const Point3& worldPos) const;
        bool UniformGrid::gridLocate(const Ray& ray, Point3ui* pt) const;
        bool contains(const Point3& pos) const;
        bool inGrid(const Point3ui& pt) const;

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;

        const GridObject& object(uint32 objId) const;

    private:
        // Objects only get stored once in the grid and are
        // referenced by their ID (implied by the position on the array)
        std::unique_ptr<GridObject[]> _objs;
        std::unique_ptr<Voxel[]> _grid;
        std::vector<GridObject> _unboundedObjs;
        Scene const* _scene;
        uint32 _numObjs;              // Number of in scene
        Bounds3 _bounds;              // Bounds of the grid
        Vec3ui _dims;                 // Number of voxels in each axis
        Vec3 _invSize;                // Inverse of size in each dimension (world units)
        Vec3 _invDims;                
    };

}