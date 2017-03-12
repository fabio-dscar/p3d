#pragma once

#include <string>
#include <vector>

namespace Photon {

    // Forward declaration
    class Scene;

    namespace Utils {

        Scene* parseNFF(const std::string& filePath);

        bool readFileToBuffer(const std::string& filePath, std::ios_base::openmode mode, std::vector<unsigned char>& buffer);
        bool readFileToString(const std::string& filePath, std::ios_base::openmode mode, std::string& str);
    
        bool isOpenGLError();
        void checkOpenGLError(const std::string& error);
        void throwError(const std::string& error);
    }
}