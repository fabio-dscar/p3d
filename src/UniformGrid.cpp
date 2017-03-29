#include <UniformGrid.h>

#include <unordered_map>

#include <Scene.h>
#include <Bounds.h>
#include <MathDefs.h>

using namespace Photon;
using namespace Photon::Math;

/*bool Voxel::intersectObjects(const Ray& ray, SurfaceEvent* evt, const UniformGrid& grid) const {
    for (uint32 id : _objIDs)
        grid.object(id).obj->intersectRay(ray, evt);

    return evt->hit();
}

bool Voxel::isOccluded(const Ray& ray, const UniformGrid& grid) const {
    for (uint32 id : _objIDs)
        if (grid.object(id).obj->isOccluded(ray))
            return true;

    return false;
}*/

void Voxel::addObject(uint32 objID) {
    objIDs.push_back(objID);
}

UniformGrid::UniformGrid(const Scene& scene, const Vec3ui& dims)
    : _dims(dims), _scene(&scene) {

    _bounds = _scene->bounds();

    Vec3 size = (_bounds.max() - _bounds.min());
    _invSize = Vec3(1.0 / size.x, 1.0 / size.y, 1.0 / size.z);
    _numObjs = (uint32)_scene->getGeometry().size();

    _invDims = Vec3(1.0 / _dims.x, 1.0 / _dims.y, 1.0 / _dims.z);
    _grid = std::make_unique<Voxel[]>(_dims.x * _dims.y * _dims.z);   
}

UniformGrid::UniformGrid(const Scene& scene, Float m)
    : _scene(&scene) {

    _bounds = _scene->bounds();

    Vec3 size = (_bounds.max() - _bounds.min());
    _invSize = Vec3(1.0 / size.x, 1.0 / size.y, 1.0 / size.z);
    _numObjs = (uint32)_scene->getGeometry().size();

    Float vol = size.cube();
    Float s = std::cbrt(vol / _numObjs);
    _dims = Vec3ui(m * size.x / s + 1, m * size.y / s + 1, m * size.z / s + 1);
    _invDims = Vec3(1.0 / _dims.x, 1.0 / _dims.y, 1.0 / _dims.z);

    _grid = std::make_unique<Voxel[]>(_dims.x * _dims.y * _dims.z);
}


void UniformGrid::initialize() {
    const std::vector<std::shared_ptr<Geometry>>& sceneObjs = _scene->getGeometry();

    // Initialize cells
    for (uint32 vx = 0; vx < _dims.x; vx++) 
        for (uint32 vy = 0; vy < _dims.y; vy++) 
            for (uint32 vz = 0; vz < _dims.z; vz++)   
                voxel(vx, vy, vz) = Voxel();

    // Build object list and set them into their cells
    _objs = std::make_unique<GridObject[]>(_numObjs);
    for (uint32 id = 0; id < _numObjs; id++) {
        _objs[id] = sceneObjs[id];

        Bounds3 objBbox = _objs[id]->bounds();

        // Check if object is unbounded
        if (!objBbox.bounded()) {
            _unboundedObjs.push_back(_objs[id]);

            // Go to next
            continue;
        }

        // Get indices of object bbox on grid 
        // (note that this is a vectorial clamp)
        Vec3ui idxMin = clamp<uint32>((objBbox.min() - _bounds.min()) * _dims * _invSize, 0, _dims - 1);
        Vec3ui idxMax = clamp<uint32>((objBbox.max() - _bounds.min()) * _dims * _invSize, 0, _dims - 1);

        // Set each object in respective voxels
        for (uint32 ix = idxMin.x; ix <= idxMax.x; ix++)
            for (uint32 iy = idxMin.y; iy <= idxMax.y; iy++)
                for (uint32 iz = idxMin.z; iz <= idxMax.z; iz++)
                    voxel(ix, iy, iz).addObject(id); 
    }
}

Voxel& UniformGrid::voxel(uint32 x, uint32 y, uint32 z) {
    return _grid[x + _dims.x * y + _dims.x * _dims.y * z];
}

const Voxel& UniformGrid::voxel(uint32 x, uint32 y, uint32 z) const {
    return _grid[x + _dims.x * y + _dims.x * _dims.y * z];
}

const Voxel& UniformGrid::voxel(const Point3& pos) const {
    Vec3 loc = (pos - _bounds.min()) * _dims * _invSize;
    Vec3ui cell = clamp<uint32>(loc, 0, _dims - 1);

    return voxel(cell.x, cell.y, cell.z);
}

const Voxel& UniformGrid::voxel(const Point3ui& pt) const {
    return _grid[pt.x + _dims.x * pt.y + _dims.x * _dims.y * pt.z];
}

Point3ui UniformGrid::gridPos(const Point3& worldPos) const {
    Vec3 pos = (worldPos - _bounds.min()) * _dims * _invSize;
    Vec3ui cell = clamp<uint32>(pos, 0, _dims - 1);

    return Point3i(cell.x, cell.y, cell.z);
}

bool UniformGrid::gridLocate(const Ray& ray, Point3ui* pt) const {
    Float t0 = 0, t1;
    if (!_bounds.contains(ray.origin()))
        if (!_bounds.intersectPts(ray, &t0, &t1))
            return false;

    Vec3 pos = (ray(t0) - _bounds.min()) * _dims * _invSize;
    Vec3ui cell = Math::clamp<uint32>(pos, 0, _dims - 1);

    *pt = Point3i(cell.x, cell.y, cell.z);

    return true;
}

bool UniformGrid::contains(const Point3& pos) const {
    return _bounds.contains(pos);
}

bool UniformGrid::inGrid(const Point3ui& pt) const {
    return (pt.x >= 0 && pt.y >= 0 && pt.z >= 0 &&
            pt.x < _dims.x && pt.y < _dims.y && pt.z < _dims.z);
}

bool UniformGrid::intersectRay(const Ray& ray, SurfaceEvent* evt) const {
    // Intersection cache, renewed per ray
    std::unordered_map<uint32, bool> intersectMap;

    // Test intersection with unbounded
    for (std::shared_ptr<Geometry> obj : _unboundedObjs)
        obj->intersectRay(ray, evt);

    // Get location of ray in grid
    Point3ui pt;
    if (!gridLocate(ray, &pt))
        return false;

    // Solutions for intersections with grid's planes
    Vec3 invRayDir = Vec3(1.0 / ray.dir().x, 1.0 / ray.dir().y, 1.0 / ray.dir().z);
    Vec3 sol1 = (_bounds.min() - ray.origin()) * invRayDir;
    Vec3 sol2 = (_bounds.max() - ray.origin()) * invRayDir;

    Vec3 tMin = Math::min(sol1, sol2);
    Vec3 tMax = Math::max(sol1, sol2);

    // Calculate delta in each dimension
    Vec3 dt = (tMax - tMin) * _invDims;
  
    // Set step and initial t for traversal
    Vec3i step = Math::sign(ray.dir());
    Vec3 adv = dt * (Point3i(step.x > 0 ? (pt.x + 1) : (_dims.x - pt.x),
                             step.y > 0 ? (pt.y + 1) : (_dims.y - pt.y),
                             step.z > 0 ? (pt.z + 1) : (_dims.z - pt.z))).posVec();
    Vec3 t = tMin + adv;

    // Start grid traversal
    Voxel vox;
    uint32 min;
    do {
        min = t.minDim(); // Get component with smalles value
        vox = voxel(pt);

        /*if (vox.hasObjects() && vox.intersectObjects(ray, evt, *this) && ray.maxT() < t[min])
            return true;*/

        if (vox.hasObjects()) {
            for (uint32 id : vox.objIDs) {
                // Only intersect if not already
                if (intersectMap.find(id) == intersectMap.end())
                    if (_objs[id]->intersectRay(ray, evt))
                        intersectMap[id] = true;
            }

            if (ray.maxT() < t[min])
                return true;
        }

        t[min]  += dt[min];
        pt[min] += step[min];

    } while (pt[min] >= 0 && pt[min] < _dims[min]);

    return evt->hit();
}

bool UniformGrid::isOccluded(const Ray& ray) const {
    // Test occlusion with unbounded
    for (GridObject obj : _unboundedObjs)
        if (obj->isOccluded(ray))
            return true;

    // Locate ray on the grid
    Point3ui pt;
    if (!gridLocate(ray, &pt))
        return false;

    // Solutions for intersections with grid's planes
    Vec3 invRayDir = Vec3(1.0 / ray.dir().x, 1.0 / ray.dir().y, 1.0 / ray.dir().z);
    Vec3 sol1 = (_bounds.min() - ray.origin()) * invRayDir;
    Vec3 sol2 = (_bounds.max() - ray.origin()) * invRayDir;

    Vec3 tMin = Math::min(sol1, sol2);
    Vec3 tMax = Math::max(sol1, sol2);

    // Calculate delta in each dimension
    Vec3 dt = (tMax - tMin) * _invDims;

    // Set step and initial t for traversal
    Vec3i step = Math::sign(ray.dir());
    Vec3 adv = dt * (Point3i(step.x > 0 ? (pt.x + 1) : (_dims.x - pt.x),
                             step.y > 0 ? (pt.y + 1) : (_dims.y - pt.y),
                             step.z > 0 ? (pt.z + 1) : (_dims.z - pt.z))).posVec();
    Vec3 t = tMin + adv;

    // Start grid traversal
    Voxel vox;
    uint32 min;
    do {
        min = t.minDim(); // Get component with smalles value
        vox = voxel(pt);
        
        if (vox.hasObjects()) {
            for (uint32 id : vox.objIDs)
                if (_objs[id]->isOccluded(ray))
                    return true;
        }

        t[min]  += dt[min];
        pt[min] += step[min];
        
    } while (pt[min] >= 0 && pt[min] < _dims[min]);

    return false;
}

const GridObject& UniformGrid::object(uint32 objId) const {
    return _objs[objId];
}