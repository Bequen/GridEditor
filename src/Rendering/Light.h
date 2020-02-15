#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#define LIGHT_TYPE_DIRECTIONAL 0.0f
#define LIGHT_TYPE_POINT 1.0f
#define LIGHT_TYPE_SPOT 2.0f


/**
 * @brief  Light source inside of the scene
 * @note   Each variable stores something special in it's 4th parameter
 * @retval None
 */
struct Light {
    // Position + light type
    glm::vec4 position;
    // Direction + attenuation
    glm::vec4 direction;

    // Ambient + strength
    glm::vec4 ambient;
};