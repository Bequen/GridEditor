#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct ExtrusionInfo {
    uint32_t* indices;
    uint32_t count;
    uint32_t max;
};