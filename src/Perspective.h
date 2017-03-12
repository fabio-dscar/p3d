#pragma once

#include <Camera.h>
#include <glm\glm.hpp>
#include <glm\mat4x4.hpp>
#include <glm\vec3.hpp>

namespace Invaders {

    class Logger;

    class Perspective : public Camera {
    public:
        Perspective(unsigned int width, unsigned int height, float fov, float near, float far,
                    glm::vec3 eye, const glm::vec3 up, const glm::vec3 target);

        const glm::mat4& calcProjMatrix();

        float getFov() const;
        void log(Logger& logger);

        void update() { }
    private:
        float _fov;
    };

}