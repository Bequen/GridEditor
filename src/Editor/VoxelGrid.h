#pragma once

#include <cstdint>

#include "System/Grid3D.h"

struct VoxelGrid {
    uint32_t texture;
    uint32_t size;

    VoxelGrid();
    VoxelGrid(uint32_t size);

    void remove();
};