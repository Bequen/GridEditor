#pragma once

#include <cstdint>

#include "Grid.h"
#include "Rendering/Light.h"
#include "VoxelGrid.h"
#include "Rendering/LightBuffer.h"

#define INITIAL_GRID_COUNT 8
#define INITIAL_LIGHT_COUNT 32


struct Scene {
    VoxelGrid* grids;
    uint32_t gridSize;
    uint32_t gridCount;

    Light* lights;
    uint32_t lightBuffer;
    uint32_t lightCount;
    uint32_t lightBufferSize;
    glm::vec4 skyColor;

    Scene();
    Scene(uint32_t gridCount);
    Scene(uint32_t gridCount, uint32_t lightCount);

    void init();
};