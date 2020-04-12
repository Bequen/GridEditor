#pragma once

#include <cstdint>

#include "Viewport/RayHit.h"
#include "System/Window.h"

/**
 * @brief  Ray is simple vector going from point to direction
 * @note   Ray is often used to determine what is in front of user in the scene
 * @retval None
 */
struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};