#pragma once

#include <cstdint>

#include "Grid.h"
#include "Rendering/Light.h"
#include "VoxelGrid.h"
#include "Rendering/LightBuffer.h"
#include "Editor/Color.h"

#define INITIAL_GRID_COUNT 8
#define INITIAL_LIGHT_COUNT 32


struct Scene {
    VoxelGrid* grids;
    uint32_t gridSize;
    uint32_t gridCount;

    uint32_t paletteTexture;

    Light* lights;
    uint32_t lightBuffer;
    uint32_t lightCount;
    uint32_t lightBufferSize;
    glm::vec4 skyColor;

    RGB32* palette;
    uint32_t colorSelected;
    uint32_t colorCache;

    uint32_t voxelVAO;
    uint32_t boxShader;
    uint32_t quadShader;

    void init(uint32_t gridCount);
};