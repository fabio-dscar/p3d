#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <memory>

#include <Vector.h>
#include <Material.h>

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
            static void parsePolygon(Scene& scene);
            static void parsePolygonPatch(Scene& scene);
            static void parseMaterial(Scene& scene);
            static void parseCamera(Scene& scene);

            static float      parseFloat();
            static int        parseInt();
            static const Vec2 parseVector2();
            static const Vec3 parseVector3();

            static bool isBufferEmpty();
            static bool loadLine();

            static std::ifstream _buffer;
            static std::istringstream _lineBuffer;
            static Material _material;
        };
    
    }
}