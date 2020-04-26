#pragma once

#include <cstdint>

#include "System/Grid3D.h"

struct VoxelGrid {
    uint32_t texture;
    uint32_t width, height, depth;

    VoxelGrid();
    VoxelGrid(uint32_t size);
    VoxelGrid(uint32_t width, uint32_t height, uint32_t depth);
};