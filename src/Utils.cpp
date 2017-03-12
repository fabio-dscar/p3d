#include <Utils.h>
#include <fstream>
#include <iostream>
#include <GL/glew.h>

#include <Scene.h>

#include <sstream>

using namespace Photon;

const Vec3 parseVector3(const std::string& line) {
    std::istringstream istr(line);

    float x, y, z;
    istr >> x;
    istr >> y;
    istr >> z;

    return Vec3(x, y, z);
}

void parseLight(Scene* scene, const std::string& line) {
    std::istringstream istr(line.substr(1));

    PointLight l;

    Vec3 v = parseVector3(istr.str());

    float x, y, z;
    istr >> x;
    istr >> y;
    istr >> z;

    l.setPosition(Vec3(x, y, z));

    if (istr.rdbuf()->in_avail()) {
        istr >> x;
        istr >> y;
        istr >> z;

        l.setColor(Color3(x, y, z));
    }

    scene->addLight(l);
}

void parseView(Scene* scene, std::ifstream& file) {
    std::string line;

    Camera cam;

    /*while (std::getline(file, line)) {
        if (line.compare(0, 1, "from") == 0) {
            cam.setPosition(parseVector3(line.substr(4)));
        } 
        else if (line.compare(0, 1, "up") == 0) {
            cam.setUp(parseVector3(line.substr(2)));
        }
    }*/
}

Scene* Photon::Utils::parseNFF(const std::string& filePath) {
    std::ifstream file(filePath, std::ios_base::in);
    if (file.fail()) {
        perror(filePath.c_str());
        throwError("Couldn't read file " + filePath);
    }

    Scene* scene = new Scene();

    std::string line;

    while (std::getline(file, line)) {

        // Background color
        if (line.compare(0, 1, "b") == 0) {
            std::istringstream istr(line.substr(1));

            float r, g, b;
            istr >> r;
            istr >> g;
            istr >> b;

            scene->setBackgroundColor(Color3(r, g, b));
        } else if (line.compare(0, 1, "l") == 0) {
            parseLight(scene, line);
        } else if (line.compare(0, 1, "v") == 0) {
            parseView(scene, file);
        }

    }

    file.close();

    return scene;
}

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