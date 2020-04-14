#pragma once

#include <cstdint>

#include "Editor/Color.h"
#include "Rendering/Light.h"
#include "System/Voxels/Grid.h"
#include "Editor/SceneObject.h"

#define INITIAL_GRID_COUNT 8
#define INITIAL_LIGHT_COUNT 32
#define MAX_LIGHT_COUNT 32

struct Scene {
    #pragma region Grids
    Grid* grids;
    uint32_t gridCount;
    uint32_t gridBufferSize;
    #pragma endregion

    // TODO Move into separate structure
    #pragma region Lights
    Light* lights;
    uint32_t lightBuffer;
    uint32_t lightCount;
    uint32_t lightBufferSize;
    glm::vec4 skyColor;
    #pragma endregion

    #pragma region Palette
    uint32_t paletteTexture;
    RGB32* palette;
    int8_t colorSelected;
    int8_t colorCache;
    #pragma endregion

    #pragma region Scene objects
    SceneObject sceneGraph;
    SceneObject* selected;
    bool isEditMode;
    #pragma endregion

    void init(uint32_t gridCount);

    Grid* add_grid(Grid grid);
    Light* add_light(Light light);

    void update_lights();

    void assign_grid(SceneObject* sceneObject, uint32_t gridID);
    void assign_light(SceneObject* SceneObject, uint32_t lightID);
};