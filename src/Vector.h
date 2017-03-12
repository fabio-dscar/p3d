#pragma once

#include <glm\glm.hpp>

#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>

#include <Definitions.h>

namespace Photon {

#ifdef USE_GLM
    typedef glm::vec2 Vec2;
    typedef glm::vec3 Vec3;
    typedef glm::vec4 Vec4;

    typedef glm::vec3 Color3;
    typedef glm::vec4 Color4;

    typedef glm::uvec2 Vec2u;
#endif

}