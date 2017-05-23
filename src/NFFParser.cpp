#include <NFFParser.h>

#include <memory>
#include <filesystem>

#include <Utils.h>
#include <Scene.h>
#include <Sphere.h>
#include <Cylinder.h>
#include <Plane.h>
#include <Polygon.h>
#include <PolygonPatch.h>
#include <Triangle.h>
#include <Box.h>
#include <Quad.h>
#include <TriMesh.h>

#include <PointLight.h>
#include <AreaLight.h>
#include <Spectral.h>

#include <BSDF.h>
#include <Lambertian.h>
#include <OrenNayar.h>
#include <Phong.h>
#include <Specular.h>
#include <ThinSpecular.h>
//#include <Mirror.h>
#include <RoughSpecular.h>
#include <MicrofacetRefraction.h>
#include <MicrofacetReflection.h>
#include <Conductor.h>
#include <AshikhminShirley.h>
#include <SmoothLayered.h>

#include <Resources.h>

#include <StratifiedSampler.h>
#include <RandomSampler.h>

using namespace Photon;
using namespace Photon::Utils;

// Static attributes
std::ifstream NFFParser::_buffer = std::ifstream();
std::istringstream NFFParser::_lineBuffer = std::istringstream();
BSDF* NFFParser::_bsdf = nullptr;
MatrixStack NFFParser::_matStack = MatrixStack();

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

        // Matrix stack commands
        if (cmd.compare(0, 3, "pop") == 0) {
            _matStack.popMatrix();
        } else if (cmd.compare(0, 4, "push") == 0) {
            _matStack.pushMatrix();
        } else if (cmd.compare(0, 8, "identity") == 0) {
            _matStack.loadIdentity();
        } else if (cmd.compare(0, 9, "translate") == 0) {
            Vec3 tr = parseVector3();
            _matStack.translate(tr);
        } else if (cmd.compare(0, 5, "scale") == 0) {
            Vec3 s = parseVector3();
            _matStack.scale(s);
        } else if (cmd.compare(0, 7, "rotateX") == 0) {
            Float angle = parseFloat();
            _matStack.rotateX(angle);
        } else if (cmd.compare(0, 7, "rotateY") == 0) {
            Float angle = parseFloat();
            _matStack.rotateY(angle);
        } else if (cmd.compare(0, 7, "rotateZ") == 0) {
            Float angle = parseFloat();
            _matStack.rotateZ(angle);
        }

        if (cmd.compare(0, 3, "obj") == 0) {
            parseTriMesh(*scene);
        }

        if (cmd.compare(0, 4, "grid") == 0) {
            std::string str = parseStr();
            if (str.compare(0, 3, "off") == 0)
                scene->useGrid(false);
        }

        /*if (cmd.compare(0, 7, "sampler") == 0) {

            std::string str = parseStr();
            if (str.compare(0, 10, "stratified") == 0) {
                uint32 nx = parseInt();
                uint32 ny = parseInt();

                _sampler = std::make_unique<StratifiedSampler>(nx, ny, 6);
            } else if (str.compare(0, 6, "random") == 0) {
                uint32 spp = parseInt();

                _sampler = std::make_unique<RandomSampler>(spp, 6);
            }
               
        }*/

        if (cmd.compare(0, 3, "box") == 0) {
            parseBox(*scene);
        } else if(cmd.compare(0, 4, "tone") == 0) {
            std::string op = parseStr();
            if (op.compare(0, 6, "linear") == 0) {
                scene->getCamera().film().setToneOperator(LINEAR);
            } else {
                Float exposure = parseFloat();
                scene->getCamera().film().setToneOperator(FILMIC);
                scene->getCamera().film().setExposure(exposure);
            }
        } else if (cmd.compare(0, 2, "qd") == 0) {
            parseQuad(*scene);
        } else if (cmd.compare(0, 4, "bsdf") == 0) {
            parseBsdf(*scene);
        } else if (cmd.compare(0, 1, "b") == 0) {
            scene->setBackgroundColor(parseColor());
        } else if (cmd.compare(0, 1, "l") == 0) {
            parseLight(*scene);
        } else if (cmd.compare(0, 3, "als") == 0) {
            parseSphericalLight(*scene);
        } else if (cmd.compare(0, 3, "alp") == 0) {
            parsePlanarLight(*scene);
        } else if (cmd.compare(0, 1, "v") == 0) {
            parseCamera(*scene);
        } else if (cmd.compare(0, 1, "s") == 0) {
            parseSphere(*scene);
        } else if (cmd.compare(0, 1, "c") == 0) {
            parseCylinder(*scene);
        } else if (cmd.compare(0, 2, "pl") == 0) {
            parsePlane(*scene);
        } else if (cmd.compare(0, 1, "f") == 0) {
            ; //parseMaterial(*scene);
        } else if (cmd.compare(0, 1, "p") == 0) {
            parsePolygon(*scene);
        } else if (cmd.compare(0, 2, "pp") == 0) {
            parsePolygonPatch(*scene);
        } else if (cmd.compare(0, 1, "#") == 0) {
            // Comment, do nothing
        } else {
//            throwError("Unknown command found in scene file.");
        }
    }

    // Close and clear buffers
    _buffer.close();
    _lineBuffer.clear();

    return std::move(scene);
}

void NFFParser::parseBsdf(Scene& scene) {
    std::string bsdfName = parseStr();

    BSDF* bsdf = nullptr;

    if (bsdfName.compare(0, 10, "Lambertian") == 0) {
        Color rho = parseColor();
        bsdf = new Lambertian(rho);
    } else if (bsdfName.compare(0, 9, "OrenNayar") == 0) {
        Color rho   = parseColor();
        Float sigma = parseFloat();
        bsdf = new OrenNayar(rho, sigma);
    } else if (bsdfName.compare(0, 5, "Phong") == 0) {
        Color kd = parseColor();
        Color ks = parseColor();
        Float alpha = parseFloat();

        bsdf = new Phong(kd, ks, alpha);
    } else if (bsdfName.compare(0, 8, "Specular") == 0) {
        Float intEta = parseFloat();
        Float extEta = parseFloat();
        Color refl   = parseColor();
        Color refr   = parseColor();
        bsdf = new Specular(intEta, extEta, refl, refr);
        //bsdf = new RoughSpecular(BECKMANN, 0.2, 1.5, 1.0);
        //bsdf = new MicrofacetReflection(GGX, Vec2(0.0001, 0.003), intEta, extEta);
        //bsdf = new MicrofacetRefraction(GGX, Vec2(0.05, 0.001), intEta, extEta);

        //bsdf = new Mirror();
        //std::cout << "Specular BSDF acquired!" << std::endl;
    } else if (bsdfName.compare(0, 9, "Conductor") == 0) {
        Color refl = parseColor();
        Color eta = parseColor();
        Color k = parseColor();
        bsdf = new Conductor(refl, eta, k);
    } else if (bsdfName.compare(0, 8, "AShirley") == 0) {
        DistributionType type;
        std::string name = parseStr();
        if (name.compare(0, 3, "GGX") == 0)
            type = GGX;
        else
            type = BECKMANN;

        Color diff = parseColor();
        Color spec = parseColor();

        Vec2 alpha = parseVector2();

        bsdf = new AshikhminShirley(BECKMANN, alpha, diff, spec);
    } else if (bsdfName.compare(0, 10, "Microfacet") == 0) {
        DistributionType type;
        std::string name = parseStr();
        if (name.compare(0, 3, "GGX") == 0)
            type = GGX;
        else
            type = BECKMANN;

        Vec2 alpha = parseVector2();
        Float intIor = parseFloat();
        Float extIor = parseFloat();

        bsdf = new MicrofacetReflection(type, alpha, intIor, extIor);
    } else if (bsdfName.compare(0, 7, "Layered") == 0) {
        Color refl = parseColor();
        Float intIor = parseFloat();
        Float extIor = parseFloat();

        bsdf = new SmoothLayered(*_bsdf, refl, intIor, extIor);
    }

    _bsdf = bsdf;
}

void NFFParser::parseLight(Scene& scene) {

    // Parse position
    Point3 v = parsePoint3();
  
    Light* l = new PointLight(v);

    // Parse color if available
    if (!isBufferEmpty())
        l->setIntensity(parseColor());

    // Add light to scene
    scene.addLight(l);
}

void NFFParser::parseSphere(Scene& scene) {
    Point3 pos;
    Float radius;

    pos = parsePoint3();
    radius = parseFloat();

    std::shared_ptr<Sphere> s = std::make_shared<Sphere>(pos, radius);
    s->setBsdf(_bsdf);

    // Add sphere
    scene.addShape(s);
}

void NFFParser::parseCylinder(Scene& scene) {
    loadLine();
    Point3 base = parsePoint3();
    Float baseRadius = parseFloat();

    loadLine();
    Point3 apex = parsePoint3();
    Float apexRadius = parseFloat();

    std::shared_ptr<Cylinder> c = std::make_shared<Cylinder>(base, baseRadius, apex, apexRadius);

    // Add cylinder
    scene.addShape(c);
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

    Normal normal = Normal(normalize(cross(p1p2, p1p3)));

    // Project an arbitrary position vector on the plane into the unit normal
    Float dist = dot(normal, p1);

    std::shared_ptr<Plane> pl = std::make_shared<Plane>(normal, dist);
    pl->setBsdf(_bsdf);

    // Add plane
    scene.addShape(pl);
}

void NFFParser::parseQuad(Scene& scene) {
    // Find the normal of the plane
    Point3 origin = parsePoint3();
    Vec3 edge1 = parseVector3();
    Vec3 edge2 = parseVector3();

    std::shared_ptr<Quad> quad = std::make_shared<Quad>(origin, edge1, edge2);
    quad->setBsdf(_bsdf);

    // Add plane
    scene.addShape(quad);
}

void NFFParser::parsePolygon(Scene& scene) {
    int numPts = parseInt();
    if (numPts < 3)
        throwError("Invalid polygon description.");

    // Calculate plane normal vector
    loadLine();
    Point3 p1 = parsePoint3();
    loadLine();
    Point3 p2 = parsePoint3();
    loadLine();
    Point3 p3 = parsePoint3();

    Vec3 p1p2 = p2 - p1;
    Vec3 p1p3 = p3 - p1;

    // Check if edges make non-zero angle
    Float angle = dot(p1p2, p1p3);
    if (std::abs(angle) < 1e-10)
        throwError("Invalid polygon description.");

    Normal normal = Normal(normalize(cross(p1p2, p1p3)));

    if (numPts == 3) {
        std::shared_ptr<Triangle> tri = std::make_shared<Triangle>(p1, p2, p3, normal);

        // Add triangle
        tri->setBsdf(_bsdf);

        scene.addShape(tri);
    } else {
        std::shared_ptr<Polygon> pol = std::make_shared<Polygon>();

        pol->addVertex(p1);
        pol->addVertex(p2);
        pol->addVertex(p3);

        pol->setNormal(normal);

        // Parse the other vertices, if any
        for (int i = 0; i < numPts - 3; i++) {
            loadLine();
            Vec3 v = parseVector3();
            pol->addVertex(Point3(v.x, v.y, v.z));
        }

        // Add polygon
        scene.addShape(pol);
    }
}

void NFFParser::parsePolygonPatch(Scene& scene) {
    int numPts = parseInt();
    if (numPts < 3)
        throwError("Invalid polygon description.");

    // Calculate plane normal vector
    loadLine();
    Vertex p1 = { parsePoint3(), Normal(parseVector3()) };
    loadLine();
    Vertex p2 = { parsePoint3(), Normal(parseVector3()) };
    loadLine();
    Vertex p3 = { parsePoint3(), Normal(parseVector3()) };

    Vec3 p1p2 = p2.vertex - p1.vertex;
    Vec3 p1p3 = p3.vertex - p1.vertex;

    // Check if edges make non-zero angle
    Float angle = dot(p1p2, p1p3);
    if (std::abs(angle) < 1e-9f)
        throwError("Invalid polygon description.");

    Normal normal = Normal(normalize(cross(p1p2, p1p3)));

    std::shared_ptr<PolygonPatch> pol = std::make_shared<PolygonPatch>();
    pol->setNormal(normal);

    for (int i = 0; i < numPts - 3; i++) {
        loadLine();
        pol->addVertex(parsePoint3(), Normal(parseVector3()));
    }

    // Add polygon
    scene.addShape(pol);
}

void NFFParser::parseTriMesh(Scene& scene) {
    std::string path = parseStr();
    std::string name = parseStr();

    // Load mesh
    auto mesh = Resources::get().loadObj(path, name);
    mesh->setTransform(Transform(_matStack.loadMatrix()));
    mesh->setBsdf(_bsdf);

    // Add triangles to the scene
    const auto tris = mesh->getTris();
    for (auto obj : tris)
        scene.addShape(obj);
}

void NFFParser::parseBox(Scene & scene) {
    Point3 min = parsePoint3();
    Point3 max = parsePoint3();

    std::shared_ptr<Box> box = std::make_shared<Box>(min, max);
    box->setBsdf(_bsdf);

    scene.addShape(box);
}

/*void NFFParser::parseMaterial(Scene& scene) {
    Color color = parseColor();
    Float diff = parseFloat();
    Float spec = parseFloat();
    Float shininess = parseFloat();
    Float transmit = parseFloat();
    Float ior = parseFloat();

    Material mtl(color, diff, spec, shininess, transmit, ior);

    _material = mtl;
}*/

void NFFParser::parsePlanarLight(Scene& scene) {

}

void NFFParser::parseSphericalLight(Scene& scene) {
    // Parse the sphere shape
    Point3 pos = parsePoint3();
    Float radius = parseFloat();

    std::shared_ptr<Sphere> s = std::make_shared<Sphere>(pos, radius);

    // Parse light emission
    Color emission = parseColor();
    uint32 nSamples = parseInt();

    Light* light = new AreaLight(s, emission, nSamples);
    s->setLight((AreaLight*)light);

    // Add sphere
    scene.addShape(s);
}

#include <Perspective.h>

void NFFParser::parseCamera(Scene& scene) {
    std::string cmd;
    Point3 from, target;
    Vec3 up;
    Float fov, near;
    Vec2ui res;
    Float radius = 0;
    Float focalDist = 0;

    // 'from' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 4, "from") == 0)
        from = parsePoint3();

    // 'at' line
    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 2, "at") == 0)
        target = parsePoint3();

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

    loadLine();
    _lineBuffer >> cmd;
    if (cmd.compare(0, 4, "lens") == 0) {
        radius = parseFloat();
        focalDist = parseFloat();
    }

    Transform camToWorld = Camera::lookAt(from, target, up);

    Camera* cam = new Perspective(camToWorld, res, fov, near, 1000); // Camera(from, up, target, fov, near, res);
    cam->setLensParams(radius, focalDist);

    scene.addCamera(*cam);
}

Float NFFParser::parseFloat() {
    Float x;
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
    Float x, y;
    _lineBuffer >> x;
    _lineBuffer >> y;

    if (_lineBuffer.fail())
        throwError("Failed to parse file.");

    return Vec2(x, y);
}

const Vec3 NFFParser::parseVector3() {
    Float x, y, z;
    _lineBuffer >> x;
    _lineBuffer >> y;
    _lineBuffer >> z;

    if (_lineBuffer.fail())
        throwError("Failed to parse file.");

    return Vec3(x, y, z);
}

const Color NFFParser::parseColor() {
    Float r, g, b;
    _lineBuffer >> r;
    _lineBuffer >> g;
    _lineBuffer >> b;

    if (_lineBuffer.fail())
        throwError("Failed to parse file.");

    return Color(r, g, b);
}

const Point3 NFFParser::parsePoint3() {
    Float x, y, z;
    _lineBuffer >> x;
    _lineBuffer >> y;
    _lineBuffer >> z;

    if (_lineBuffer.fail())
        throwError("Failed to parse file.");

    return Point3(x, y, z);
}

const std::string NFFParser::parseStr() {
    std::string str;

    _lineBuffer >> str;

    return str;
}