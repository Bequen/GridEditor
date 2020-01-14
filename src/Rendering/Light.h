#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#define LIGHT_TYPE_DIRECTIONAL 0.0f
#define LIGHT_TYPE_POINT 1.0f
#define LIGHT_TYPE_SPOT 2.0f

struct Light {
    glm::vec4 position;
    glm::vec4 direction;

    glm::vec4 ambient;
};