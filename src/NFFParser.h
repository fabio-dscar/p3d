#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <memory>

#include <Vector.h>
#include <MatrixStack.h>
#include <BSDF.h>
#include <PhotonMath.h>

namespace Photon {

    // Forward declaration
    class Scene;

    namespace Utils {

        class NFFParser {
        public:
            static std::shared_ptr<Scene> fromFile(const std::string& filePath);

        private:
            static void parseLight(Scene& scene);
            static void parseSphere(Scene& scene);
            static void parseCylinder(Scene& scene);
            static void parsePlane(Scene& scene);
            static void parseQuad(Scene& scene);
            static void parsePolygon(Scene& scene);
            static void parsePolygonPatch(Scene& scene);
            static void parseTriMesh(Scene& scene);
            static void parseBox(Scene& scene);
            //static void parseMaterial(Scene& scene);
            static void parseBsdf(Scene& scene);
            static void parseCamera(Scene& scene);

            static void parsePlanarLight(Scene& scene);
            static void parseSphericalLight(Scene& scene);

            static Float        parseFloat();
            static uint32       parseInt();
            static const Vec2   parseVector2();
            static const Vec3   parseVector3();
            static const Point3 parsePoint3();
            static const Color  parseColor();
            static const std::string parseStr();

            static bool isBufferEmpty();
            static bool loadLine();

            static BSDF* _bsdf;
            static std::ifstream _buffer;
            static std::istringstream _lineBuffer;
            static MatrixStack _matStack;
        };
    
    }
}