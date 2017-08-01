#include <Resources.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <TriMesh.h>
#include <Utils.h>

#pragma warning(disable : 4267)  // size_t to unsigned int

using namespace Photon;

std::shared_ptr<Transform> Resources::addTransform(const Transform& transform) {
    std::shared_ptr<Transform> tr = std::make_shared<Transform>(transform);
    
    _transforms.push_back(tr);
    
    return tr;
}

std::shared_ptr<TriMesh> Resources::loadObj(const std::string& path, const std::string& name) {

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    std::string filePath(path);
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filePath.c_str());

    if (!ret)
        Utils::throwError("There was an error loading model: " + path + ".");

    if (shapes.size() > 1)
        Utils::throwError("More than one model in an obj file.");

    uint32 numIndices = shapes[0].mesh.indices.size();
    uint32 numVerts   = numIndices;
    uint32 numFaces   = 0;

    std::vector<uint32> indices;
    indices.reserve(numIndices);

    std::unique_ptr<Point3[]> verts = std::make_unique<Point3[]>(numVerts);

    std::unique_ptr<Normal[]> norms = nullptr;
    if (attrib.normals.size() > 0)
        norms = std::make_unique<Normal[]>(numVerts);

    std::unique_ptr<Point2[]> uvs = nullptr;
    if (attrib.texcoords.size() > 0)
        uvs = std::make_unique<Point2[]>(numVerts);

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); s++) {

        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (int v = 0; v < fv; v++) {
                // Access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                uint32 vertIdx = 3 * idx.vertex_index;
                verts[3*f + v].x = attrib.vertices[vertIdx];
                verts[3*f + v].y = attrib.vertices[vertIdx + 1];
                verts[3*f + v].z = attrib.vertices[vertIdx + 2];

                if (attrib.normals.size() > 0) {
                    uint32 normIdx = 3 * idx.normal_index;

                    norms[3 * f + v].x = attrib.normals[normIdx];
                    norms[3 * f + v].y = attrib.normals[normIdx + 1];
                    norms[3 * f + v].z = attrib.normals[normIdx + 2];

                    norms[3 * f + v].normalize();
                }

                if (attrib.texcoords.size() > 0) {
                    uint32 uvIdx = 2 * idx.texcoord_index;

                    uvs[2 * f + v].x = attrib.texcoords[uvIdx];
                    uvs[2 * f + v].y = attrib.texcoords[uvIdx + 1];
                }

                indices.push_back(3*f + v);
            }

            ++numFaces;

            // Offset indexes
            index_offset += fv;

            // Per-face material
            shapes[s].mesh.material_ids[f];

        }
    }

    std::shared_ptr<TriMesh> mesh = std::make_shared<TriMesh>(numFaces, numVerts, &indices[0],
                                                              verts.get(), norms.get(), nullptr, uvs.get(), nullptr, Transform());

    // Insert mesh into the resource map
    _meshMap.insert(std::make_pair(name, mesh));

    return mesh;
}