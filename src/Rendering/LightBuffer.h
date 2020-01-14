#pragma once

#define MAX_LIGHT_COUNT 32

#include <cstdint>

#include "Light.h"

struct LightBuffer {
    Light lights[MAX_LIGHT_COUNT];
    glm::vec4 skyColor;

    uint32_t lightCount;
    uint32_t lightBuffer;
};