#include <Orthogonal.h>
#include <Logger.h>

#include <glm\gtc\matrix_transform.hpp>

using namespace Invaders;

Orthogonal::Orthogonal(float left, float right, float bottom, float top, float near, float far,
                       const glm::vec3 eye, const glm::vec3 up, const glm::vec3 target) :
    Camera((int)(std::abs(left) + std::abs(right)), (int)(std::abs(bottom) + std::abs(right)), eye, up, target), 
    _right(right), _left(left), _top(top), _bottom(bottom) {

    _near = near; 
    _far = far;

    calcViewMatrix();
    calcProjMatrix();
}

const glm::mat4& Orthogonal::calcProjMatrix() {
    float aspectRatio = (float)_width / (float)_height;

    float right = _right;
    float left = _left;
    float bottom = _bottom;
    float top = _top;

    if (_width > _height) {
        right = _right * aspectRatio;
        left = _left * aspectRatio;
    } else {
        bottom = _bottom * 1.0f/aspectRatio;
        top = _top * 1.0f/aspectRatio;
    }

    _projMatrix = {
        glm::vec4(2.0f / (right - left), 0.0f, 0.0f, 0.0f),
        glm::vec4(0.0f, 2.0f / (top - bottom), 0.0f, 0.0f),
        glm::vec4(0.0f, 0.0f, -2.0f / (_far - _near), 0.0f),
        glm::vec4(-(right + left) / (right - left), -(top + bottom) / (top - bottom), -(_far + _near) / (_far - _near), 1.0f)
    };

    //return glm::ortho(left, right, bottom, top, _near, _far);

    return _projMatrix;
}

void Orthogonal::log(Logger& logger) {
    logger.log(*this);
}

float Orthogonal::getRight() const {
    return _right;
}

float Orthogonal::getLeft() const {
    return _left;
}

float Orthogonal::getTop() const {
    return _top;
}

float Orthogonal::getBotom() const {
    return _bottom;
}