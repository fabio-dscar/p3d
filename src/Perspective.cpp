#include <Perspective.h>
#include <Logger.h>

using namespace Invaders;

Perspective::Perspective(unsigned int width, unsigned int height, float fov, float near, float far,
            glm::vec3 eye, const glm::vec3 up, const glm::vec3 target) :
    Camera(width, height, eye, up, target), _fov(fov) {

    _near = near;
    _far = far;

    calcViewMatrix();
    calcProjMatrix();
}

const glm::mat4& Perspective::calcProjMatrix() {
    const float aspectRatio = (float)_width / (float)_height;
    const float zRange = _far - _near;
    const float tanHalfFOV = std::tanf(glm::radians(_fov / 2.0f));

    _projMatrix = {
        glm::vec4(1.0f / (tanHalfFOV * aspectRatio), 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 1.0f / tanHalfFOV, 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, (-_near - _far) / zRange, -1.0f),
        glm::vec4(0.0f, 0.0f, -2.0f * _far * _near / zRange, 0.0f)
    };

    return _projMatrix;
}

float Perspective::getFov() const {
    return _fov;
}

void Perspective::log(Logger& logger) {
    logger.log(*this);
}