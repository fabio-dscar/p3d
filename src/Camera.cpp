#include <Camera.h>
#include <glm\ext.hpp>

/*using namespace Invaders;

const glm::mat4& Camera::calcViewMatrix() {
    glm::vec3 direction = glm::normalize(_target - getPosition());

    _pitch = asinf(-direction.y);
    _yaw = atan2f(direction.x, -direction.z);

    normalizeAngles();

    // Create the view matrix (a translation followed by a rotation)
    _viewMatrix = getOrientation() * glm::translate(glm::mat4(), -getPosition());

    return _viewMatrix;
}

void Camera::move(const glm::vec3& translation) {
    setPosition(getPosition() + translation);

    _viewMatrix = getOrientation() * glm::translate(glm::mat4(), -getPosition());
}

const glm::mat4 Camera::getOrientation() const {
    glm::mat4 orientation;
    orientation = glm::rotate(orientation, _pitch, glm::vec3(1.0f, 0.0f, 0.0f));
    orientation = glm::rotate(orientation, _yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    return orientation;
}

void Camera::rotate(float angle, const glm::vec3 & axis) {
    glm::mat3 rot = glm::rotate(angle, axis);

    setPosition(rot * getPosition());

    calcViewMatrix();
}

void Camera::rotate(float yaw, float pitch) {
    // Implements the rotation logic for the first person style and
    // spectator style camera behaviors. Roll is ignored.
    _pitch += pitch;
    _yaw += yaw;

    normalizeAngles();
    
    _viewMatrix = getOrientation() * glm::translate(glm::mat4(), -getPosition());
}

void Camera::updateViewMatrix() {
    _viewMatrix = getOrientation() * glm::translate(glm::mat4(), -getPosition());
}

void Camera::normalizeAngles() {
    _yaw = fmodf(_yaw, glm::radians(360.0f));
    //fmodf can return negative values, but this will make them all positive
    if (_yaw < 0.0f)
        _yaw += glm::radians(360.0f);

    // Prevent gimbal lock
    if (_pitch > glm::radians(89.0f))
        _pitch = glm::radians(89.0f);
    else if (_pitch < -glm::radians(89.0f))
        _pitch = -glm::radians(89.0f);
}

const glm::mat4 Camera::getVP() const {
    return getProjMatrix() * getViewMatrix();
}

const glm::mat4 Camera::getMVP(const glm::mat4& modelMatrix) const {
    // MVP = P * V * M
    return getProjMatrix() * getViewMatrix() * modelMatrix;
}

const glm::mat4 Camera::getMV(const glm::mat4 & modelMatrix) const {
    return getViewMatrix() * modelMatrix;
}

const glm::mat4& Camera::getViewMatrix() const {
    return _viewMatrix;
}

const glm::mat4& Camera::getProjMatrix() const {
    return _projMatrix;
}

void Camera::setResolution(unsigned int width, unsigned int height) {
    _height = height;
    _width = width;
}

const glm::vec3 Camera::getN() const {
    return glm::vec3(_viewMatrix[0][2], _viewMatrix[1][2], _viewMatrix[2][2]);
}

const glm::vec3 Camera::getU() const {
    return glm::vec3(_viewMatrix[0][0], _viewMatrix[1][0], _viewMatrix[2][0]);
}

const glm::vec3 Camera::getV() const {
    return glm::vec3(_viewMatrix[0][1], _viewMatrix[1][1], _viewMatrix[2][1]);
}

void Camera::setHeight(unsigned int height) {
    _height = height;
}

void Camera::setWidth(unsigned int width) {
    _width = width;
}

void Camera::lookAt(const glm::vec3& target) {
    _target = target;

    calcViewMatrix();
}

void Camera::setEye(const glm::vec3 & eye) {
    setPosition(eye);
}

const glm::vec3& Camera::getEye() const {
    return getPosition();
}

const glm::vec3 Camera::getUp() const {
    return getV();
}

const glm::vec3& Camera::getTarget() const {
    return _target;
}

const glm::vec3& Camera::getViewDir() const {
    return _viewDir;
}

float Camera::getPitch() const {
    return _pitch;
}

float Camera::getYaw() const {
    return _yaw;
}

unsigned int Camera::getWidth() const {
    return _width;
}

unsigned int Camera::getHeight() const {
    return _height;
}

float Invaders::Camera::getAspect() const {
    return (float)_width / (float)_height;
}

float Camera::getNear() const {
    return _near;
}

float Camera::getFar() const {
    return _far;
}
*/