#pragma once

#include <cstdint>

#include "Editor/Color.h"
#include "Rendering/Light.h"
#include "Voxels/Grid.h"
#include "Voxels/VoxelGrid.h"
#include "Voxels/_Grid.h"

#define INITIAL_GRID_COUNT 8
#define INITIAL_LIGHT_COUNT 32
#define MAX_LIGHT_COUNT 32

struct Scene {
    _Grid* _grids;
    char** names;
    glm::mat4* transforms;
    uint32_t gridsBufferSize;
    int32_t gridsCount;


/*     VoxelGrid* grids;
    uint32_t gridSize;
    uint32_t gridCount; */

    uint32_t paletteTexture;

    // TODO Move into separate structure
    Light* lights;
    uint32_t lightBuffer;
    uint32_t lightCount;
    uint32_t lightBufferSize;
    glm::vec4 skyColor;

    RGB32* palette;
    int8_t colorSelected;
    int8_t colorCache;

    uint32_t voxelVAO;
    uint32_t boxShader;
    uint32_t quadShader;

    void init(uint32_t gridCount);

    void add_grid(_Grid grid, char* name);
};