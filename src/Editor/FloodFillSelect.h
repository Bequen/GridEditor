#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct FloodFillSelect {
    glm::vec3 normal;

    uint32_t* selected;
    uint32_t floodFillCount;
    uint32_t normalOffset;
};