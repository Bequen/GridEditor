#pragma once

#include <cstdint>

struct Selection {
    uint32_t* selection;
    uint32_t selectedCount;
    uint32_t bufferSize;
};