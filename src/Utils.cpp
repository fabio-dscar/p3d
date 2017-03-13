#include <Utils.h>
#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include <sstream>

#include <Scene.h>

using namespace Photon;

bool Utils::readFileToBuffer(const std::string& filePath, std::ios_base::openmode mode, std::vector<unsigned char>& buffer) {
    std::ifstream file(filePath, mode);
    if (file.fail()) {
        perror(filePath.c_str());
        return false;
    }

    //seek to the end
    file.seekg(0, std::ios::end);

    //Get the file size
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    //Reduce the file size by any header bytes that might be present
    fileSize -= file.tellg();

    buffer.resize((size_t)fileSize);
    file.read((char *)&(buffer[0]), fileSize);
    file.close();

    return true;
}

bool Utils::readFileToString(const std::string& filePath, std::ios_base::openmode mode, std::string& str) {
    std::ifstream file(filePath, mode);
    if (file.fail()) {
        perror(filePath.c_str());
        return false;
    }

    file.seekg(0, std::ios::end);
    str.reserve((size_t)file.tellg());
    file.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

    return true;
}

bool Utils::isOpenGLError() {
    bool isError = false;
    GLenum errCode;
    const GLubyte *errString;
    while ((errCode = glGetError()) != GL_NO_ERROR) {
        isError = true;
        errString = gluErrorString(errCode);
        std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
    }
    return isError;
}

void Utils::checkOpenGLError(const std::string& error) {
    if (isOpenGLError()) {
        throwError(error);
    }
}

void Utils::throwError(const std::string& error) {
    std::cerr << "[ERROR]: " << error << std::endl;
    std::cerr << "Press a key to exit..." << std::endl;
    std::cin.get();
    exit(EXIT_FAILURE);
}