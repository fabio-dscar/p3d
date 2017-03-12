#include <NFFParser.h>

#include <Utils.h>
#include <Scene.h>
#include <Sphere.h>
#include <Cylinder.h>
#include <Plane.h>

#include <memory>

using namespace Photon;
using namespace Photon::Utils;

// Static attributes
Material NFFParser::_material = Material();
std::ifstream NFFParser::_buffer = std::ifstream();
std::istringstream NFFParser::_lineBuffer = std::istringstream();

std::shared_ptr<Scene> NFFParser::fromFile(const std::string& filePath) {

    // Open file
    _buffer.open(filePath, std::ios_base::in);
    if (_buffer.fail()) {
        perror(filePath.c_str());
        Utils::throwError("Couldn't read file " + filePath);
    }

    // Parse scene
    std::shared_ptr<Scene> scene = std::make_shared<Scene>();
    //Scene* scene = new Scene();
    std::string line;

    while (std::getline(_buffer, line)) {
        std::string cmd;

        // Parse new line
        _lineBuffer.clear();
        _lineBuffer.str(line);

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

        } else if (cmd.compare(0, 2, "pp") == 0) {

        } else if (cmd.compare(0, 1, "#") == 0) {
            // Comment, do nothing
        } else {
            throwError("Unknown command found in scene file.");
        }
    }

    // Close and clear buffers
    _buffer.close();
    _lineBuffer.clear();

    return scene;
}

void NFFParser::parseLight(Scene& scene) {
    PointLight l;

    // Parse position
    l.setPosition(parseVector3());

    // Parse color if available
    if (!isBufferEmpty())
        l.setColor(parseVector3());

    // Add light to scene
    scene.addLight(l);
}

void NFFParser::parseSphere(Scene& scene) {
    Vec3 pos;
    float radius;

    pos = parseVector3();
    radius = parseFloat();

    Sphere s(pos, radius);

    // Add sphere
}

void NFFParser::parseCylinder(Scene& scene) {
    loadLine();
    Vec3 base = parseVector3();
    float baseRadius = parseFloat();

    loadLine();
    Vec3 apex = parseVector3();
    float apexRadius = parseFloat();

    Cylinder c(base, baseRadius, apex, apexRadius);

    // Add cylinder
}

void NFFParser::parsePlane(Scene& scene) {
    // Here, the plane Pl is defined as 0-level surface:
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

    // Project an arbitrary position vector on the plane into the normal
    float dist = glm::dot(normal, p1); 

    Plane pl(normal, dist);

    // Add plane
}

void NFFParser::parseMaterial(Scene& scene) {
    Color3 color = parseVector3();
    float diff = parseFloat();
    float spec = parseFloat();
    float shininess = parseFloat();

    Material mtl(color, diff, spec, shininess);

    _material = mtl;
}

void NFFParser::parseCamera(Scene& scene) {
    Camera cam;

    std::string cmd;

    // 'from' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 4, "from") == 0) {
        cam.setPosition(parseVector3());
    }

    // 'at' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 2, "at") == 0) {
        cam.setTarget(parseVector3());
    }

    // 'up' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 2, "up") == 0) {
        cam.setUp(parseVector3());
    }

    // 'angle' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 5, "angle") == 0) {
        cam.setFov(parseFloat());
    }

    // 'hither' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 6, "hither") == 0) {
        cam.setNear(parseFloat());
    }

    // 'resolution' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 10, "resolution") == 0) {
        cam.setResolution(parseVector2());
    }

    scene.addCamera(cam);
}

float NFFParser::parseFloat() {
    float x;
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

bool NFFParser::isBufferEmpty() {
    return !_lineBuffer.rdbuf()->in_avail();
}

void NFFParser::loadLine() {
    std::string line;
    std::getline(_buffer, line);
    _lineBuffer.clear();
    _lineBuffer.str(line);
}