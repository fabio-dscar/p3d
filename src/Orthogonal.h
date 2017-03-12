#pragma once

#include <Camera.h>
#include <glm\glm.hpp>
#include <glm\mat4x4.hpp>
#include <glm\vec3.hpp>

namespace Invaders {

    class Orthogonal : public Camera {
    public:
        Orthogonal(float left, float right, float bottom, float top, float near, float far,
                   const glm::vec3 eye, const glm::vec3 up, const glm::vec3 target);

        float getRight() const;
        float getLeft() const;
        float getTop() const;
        float getBotom() const;

        const glm::mat4& calcProjMatrix();

        void log(Logger& logger);

        void update() { }
    private:
        float _top;
        float _bottom;
        float _left;
        float _right;
    };

}
