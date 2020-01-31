#include "Scene.h"

#include "Rendering/RenderLib.h"
#include <glad/glad.h>

Scene::Scene() :
grids(new VoxelGrid[INITIAL_GRID_COUNT]), lights(new Light[INITIAL_LIGHT_COUNT]) {
    gridSize = INITIAL_GRID_COUNT;
    gridCount = 1;

    lightBufferSize = INITIAL_LIGHT_COUNT;
    lightCount = 0;
}

Scene::Scene(uint32_t gridCount) :
grids(new VoxelGrid[gridCount]), lights(new Light[INITIAL_LIGHT_COUNT]) {
    gridSize = gridCount;
    this->gridCount = 1;

    lightBufferSize = INITIAL_LIGHT_COUNT;
    lightCount = 0;
}

Scene::Scene(uint32_t gridCount, uint32_t lightCount) :
grids(new VoxelGrid[gridCount]), lights(new Light[lightCount]) {
    gridSize = gridCount;
    this->gridCount = 1;

    lightBufferSize = lightCount;
    this->lightCount = 0;
}


void Scene::init() {
    lightBuffer = RenderLib::create_buffer_dynamic(UNIFORM_BUFFER, sizeof(Light) * lightBufferSize + sizeof(glm::vec4), lights);
    RenderLib::buffer_binding_range(lightBuffer, 2, 0, sizeof(Light) * lightBufferSize + sizeof(glm::vec4));

    for(uint32_t i = 0; i < gridSize; i++) {
        grids[i].init();
    }
    colorSelected = 2;
    palette = (RGB32*)malloc(sizeof(RGB32) * 256);
    memset(palette, 0, sizeof(RGB32) * 256);
    paletteTexture = TextureLib::create_texture_1d(256, GL_RGB, GL_RGB, palette);
    paletteTexture = paletteTexture;
}