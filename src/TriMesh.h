#pragma once

#include <array>

#include <Vector.h>
#include <Shape.h>
#include <Vertex.h>

#include <Transform.h>

namespace Photon {

    class TriMesh;

    class MeshTriangle : public Shape {
    public:
        MeshTriangle(const TriMesh& mesh, const uint32* indices);

        bool intersectRay(const Ray& ray, SurfaceEvent* evt) const;
        bool isOccluded(const Ray& ray) const;

        void computeSurfaceEvent(const Ray& ray, SurfaceEvent& evt) const;

        Bounds3 bbox() const;
    private:
        const TriMesh* _mesh;
        const uint32*  _idx;
        //std::array<uint32, 3> _idx;  // Triangle indices
    };

    class TriMesh {
    public:
        TriMesh(uint32 numFaces, uint32 numVerts, const uint32* indices,
                const Point3* verts, const Normal* norms, const Vec3* tans, const Point2* uv,
                const BSDF* bsdf, const Transform& objToWorld)
            : _numFaces(numFaces), _numVertices(numVerts), _bsdf(bsdf),
            _vertices(nullptr), _normals(nullptr), _tans(nullptr), _uv(nullptr),
            _objToWorld(objToWorld), _worldToObj(inverse(objToWorld)) {

            _indices = std::vector<uint32>();
            _indices.reserve(3 * _numFaces);
            for (uint32 i = 0; i < 3 * numFaces; i++)
                _indices.push_back(indices[i]);

            _vertices = std::make_unique<Point3[]>(numVerts);
            for (uint32 v = 0; v < numVerts; ++v)
                _vertices[v] = verts[v];

            if (uv) {
                _uv = std::make_unique<Point2[]>(numVerts);
                memcpy(_uv.get(), uv, numVerts * sizeof(Point2));
            }

            if (norms) {
                _normals = std::make_unique<Normal[]>(numVerts);
                for (uint32 n = 0; n < numVerts; ++n)
                    _normals[n] = norms[n];
            }

            if (tans) {
                _tans = std::make_unique<Vec3[]>(numVerts);
                for (uint32 t = 0; t < numVerts; ++t)
                    _tans[t] = tans[t];
            }
        }

        std::vector<std::shared_ptr<Shape>> getTris() const {
            std::vector<std::shared_ptr<Shape>> tris;
            tris.reserve(_numFaces);

            for (uint32 i = 0; i < _numFaces; i++) {
                std::shared_ptr<MeshTriangle> tri = std::make_shared<MeshTriangle>(*this, &_indices[3 * i]);
                tri->setTransform(_objToWorld, _worldToObj);
                tri->setBsdf(_bsdf);

                tris.push_back(tri);
            }

            return tris;
        }

        const Point3& vertex(uint32 idx) const {
            return _vertices[idx];
        }

        const Normal& normal(uint32 idx) const {
            return _normals[idx];
        }

        const Vec3& tangent(uint32 idx) const {
            return _tans[idx];
        }

        const Point2& uv(uint32 idx) const {
            return _uv[idx];
        }

        bool hasNormals() const {
            return _normals != nullptr;
        }

        bool hasTangents() const {
            return _tans != nullptr;
        }

        bool hasUVs() const {
            return _uv != nullptr;
        }

        void setTransform(const Transform& transform) {
            _objToWorld = transform;

            for (uint32 n = 0; n < _numVertices; n++) {
                _vertices[n] = _objToWorld(_vertices[n]);
                _normals[n]  = _objToWorld(_normals[n]);
            }
        }

        void setBsdf(const BSDF* bsdf) {
            _bsdf = bsdf;
        }

    private:
        const uint32 _numFaces;
        const uint32 _numVertices;

        std::vector<uint32>       _indices;
        std::unique_ptr<Point3[]> _vertices;
        std::unique_ptr<Normal[]> _normals;
        std::unique_ptr<Vec3[]>   _tans;
        std::unique_ptr<Point2[]> _uv;

        Transform _objToWorld;
        Transform _worldToObj;

        const BSDF* _bsdf;
    };

}