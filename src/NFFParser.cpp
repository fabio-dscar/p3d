#include <NFFParser.h>

#include <Utils.h>
#include <Scene.h>
#include <Sphere.h>
#include <Cylinder.h>
#include <Plane.h>
#include <Polygon.h>
#include <PolygonPatch.h>
#include <Triangle.h>

#include <memory>

#include <filesystem>

using namespace Photon;
using namespace Photon::Utils;

// Static attributes
Material NFFParser::_material = Material();
std::ifstream NFFParser::_buffer = std::ifstream();
std::istringstream NFFParser::_lineBuffer = std::istringstream();

bool NFFParser::isBufferEmpty() {
    return !_lineBuffer.rdbuf()->in_avail();
}

bool NFFParser::loadLine() {
    std::string line;
    std::getline(_buffer, line);
    _lineBuffer.clear();
    _lineBuffer.str(line);

    return !_buffer.eof();
}

std::shared_ptr<Scene> NFFParser::fromFile(const std::string& filePath) {
    // Open file
    _buffer.open(filePath, std::ios_base::in);
    if (_buffer.fail()) {
        perror(filePath.c_str());
        Utils::throwError("Couldn't read file " + filePath);
    }



    // Create scene
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();

    // Parse each line
    while (loadLine()) {
        std::string cmd;
        _lineBuffer >> cmd;

        if (cmd.compare(0, 1, "b") == 0) {
            scene->setBackgroundColor(parseVector3());
        } else if (cmd.compare(0, 1, "l") == 0) {
            parseLight(*scene);
        } else if (cmd.compare(0, 1, "v") == 0) {
            parseCamera(*scene);
        } else if (cmd.compare(0, 1, "s") == 0) {
            parseSphere(*scene);
        } else if (cmd.compare(0, 1, "c") == 0) {
            parseCylinder(*scene);
        } else if (cmd.compare(0, 2, "pl") == 0) {
            parsePlane(*scene);
        } else if (cmd.compare(0, 1, "f") == 0) {
            parseMaterial(*scene);
        } else if (cmd.compare(0, 1, "p") == 0) {
            parsePolygon(*scene);
        } else if (cmd.compare(0, 2, "pp") == 0) {
            parsePolygonPatch(*scene);
        } else if (cmd.compare(0, 1, "#") == 0) {
            // Comment, do nothing
        } else {
            throwError("Unknown command found in scene file.");
        }
    }

    // Close and clear buffers
    _buffer.close();
    _lineBuffer.clear();

    return std::move(scene);
}

void NFFParser::parseLight(Scene& scene) {
    std::shared_ptr<PointLight> l = std::make_shared<PointLight>();

    // Parse position
    l->setPosition(parseVector3());

    // Parse color if available
    if (!isBufferEmpty())
        l->setColor(parseVector3());

    // Add light to scene
    scene.addLight(l);
}

void NFFParser::parseSphere(Scene& scene) {
    Vec3 pos;
    float radius;

    pos = parseVector3();
    radius = parseFloat();

    std::shared_ptr<Sphere> s = std::make_shared<Sphere>(pos, radius);
    s->addMaterial(_material);

    // Add sphere
    scene.addGeometry(s);
}

void NFFParser::parseCylinder(Scene& scene) {
    loadLine();
    Vec3 base = parseVector3();
    float baseRadius = parseFloat();

    loadLine();
    Vec3 apex = parseVector3();
    float apexRadius = parseFloat();

    std::shared_ptr<Cylinder> c = std::make_shared<Cylinder>(base, baseRadius, apex, apexRadius);
    c->addMaterial(_material);

    // Add cylinder
    scene.addGeometry(c);
}

void NFFParser::parsePlane(Scene& scene) {
    // Here, the plane Pl is defined as a level 0 surface:
    // Pl = 0 <=> Pl = (norm . p) - d
    // norm - is the normal to the plane
    // p - is a point in the plane
    // d - the distance of the plane to the origin

    // Find the normal of the plane
    Vec3 p1 = parseVector3();
    Vec3 p2 = parseVector3();
    Vec3 p3 = parseVector3();

    Vec3 p1p2 = p2 - p1;
    Vec3 p1p3 = p3 - p1;

    Vec3 normal = glm::normalize(glm::cross(p1p2, p1p3));

    // Project an arbitrary position vector on the plane into the unit normal
    float dist = glm::dot(normal, p1); 

    std::shared_ptr<Plane> pl = std::make_shared<Plane>(normal, dist);
    pl->addMaterial(_material);

    // Add plane
    scene.addGeometry(pl);
}

void NFFParser::parsePolygon(Scene& scene) {
    int numPts = parseInt();
    if (numPts < 3)
        throwError("Invalid polygon description.");

    // Calculate plane normal vector
    loadLine();
    Vec3 p1 = parseVector3();
    loadLine();
    Vec3 p2 = parseVector3();
    loadLine();
    Vec3 p3 = parseVector3();

    Vec3 p1p2 = p2 - p1;
    Vec3 p1p3 = p3 - p1;

    // Check if edges make non-zero angle
    float angle = glm::dot(p1p2, p1p3);
    if (std::abs(angle) < 1e-9f)
        throwError("Invalid polygon description.");

    Vec3 normal = glm::normalize(glm::cross(p1p2, p1p3));

    if (numPts == 3) {
        std::shared_ptr<Triangle> tri = std::make_shared<Triangle>(p1, p2, p3, normal);

        // Add triangle
        tri->addMaterial(_material);
        scene.addGeometry(tri);
    } else {
        std::shared_ptr<Polygon> pol = std::make_shared<Polygon>();

        pol->setNormal(normal);

        // Parse the other vertices, if any
        for (int i = 0; i < numPts - 3; i++) {
            loadLine();
            pol->addVertex(parseVector3());
        }

        // Add polygon
        pol->addMaterial(_material);
        scene.addGeometry(pol);
    }
}

void NFFParser::parsePolygonPatch(Scene& scene) {
    int numPts = parseInt();
    if (numPts < 3)
        throwError("Invalid polygon description.");

    // Calculate plane normal vector
    loadLine();
    PatchVertex p1 = { parseVector3(), parseVector3() };
    loadLine();
    PatchVertex p2 = { parseVector3(), parseVector3() };
    loadLine();
    PatchVertex p3 = { parseVector3(), parseVector3() };

    Vec3 p1p2 = p2._vertex - p1._vertex;
    Vec3 p1p3 = p3._vertex - p1._vertex;

    // Check if edges make non-zero angle
    float angle = glm::dot(p1p2, p1p3);
    if (std::abs(angle) < 1e-9f)
        throwError("Invalid polygon description.");

    Vec3 normal = glm::normalize(glm::cross(p1p2, p1p3));

    std::shared_ptr<PolygonPatch> pol = std::make_shared<PolygonPatch>();
    pol->setNormal(normal);

    for (int i = 0; i < numPts - 3; i++) {
        loadLine();
        pol->addVertex(parseVector3(), parseVector3());
    }

    pol->addMaterial(_material);

    // Add polygon
    scene.addGeometry(pol);
}

void NFFParser::parseMaterial(Scene& scene) {
    Color3 color = parseVector3();
    float diff = parseFloat();
    float spec = parseFloat();
    float shininess = parseFloat();
    float transmit = parseFloat();
    float ior = parseFloat();

    Material mtl(color, diff, spec, shininess, transmit, ior);

    _material = mtl;
}

void NFFParser::parseCamera(Scene& scene) {
    std::string cmd;
    Vec3 from, target, up;
    float fov, near;
    Vec2u res;

    // 'from' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 4, "from") == 0)
        from = parseVector3();

    // 'at' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 2, "at") == 0)
        target = parseVector3();

    // 'up' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 2, "up") == 0)
        up = parseVector3();

    // 'angle' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 5, "angle") == 0)
        fov = parseFloat();

    // 'hither' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 6, "hither") == 0)
        near = parseFloat();

    // 'resolution' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 10, "resolution") == 0)
        res = parseVector2();

    Camera cam = Camera(from, up, target, fov, near, res);
    scene.addCamera(cam);
}

float NFFParser::parseFloat() {
    float x;
    _lineBuffer >> x;

    if (_lineBuffer.fail())
        throwError("Failed to parse file.");

    return x;
}

uint32 NFFParser::parseInt() {
    uint32 x;
    _lineBuffer >> x;

    if (_lineBuffer.fail())
        throwError("Failed to parse file.");

    return x;
}

const Vec2 NFFParser::parseVector2() {
    float x, y;
    _lineBuffer >> x;
    _lineBuffer >> y;

    if (_lineBuffer.fail())
        throwError("Failed to parse file.");

    return Vec2(x, y);
}

const Vec3 NFFParser::parseVector3() {
    float x, y, z;
    _lineBuffer >> x;
    _lineBuffer >> y;
    _lineBuffer >> z;

    if (_lineBuffer.fail())
        throwError("Failed to parse file.");

    return Vec3(x, y, z);
}