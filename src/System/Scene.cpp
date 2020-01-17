#include "Scene.h"

#include "Rendering/RenderLib.h"

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
    gridCount = 1;

    lightBufferSize = INITIAL_LIGHT_COUNT;
    lightCount = 0;
}

Scene::Scene(uint32_t gridCount, uint32_t lightCount) :
grids(new VoxelGrid[gridCount]), lights(new Light[lightCount]) {
    gridSize = gridCount;
    gridCount = 1;

    lightBufferSize = lightCount;
    this->lightCount = 0;
}


void Scene::init() {
    lightBuffer = RenderLib::create_buffer_dynamic(UNIFORM_BUFFER, sizeof(Light) * lightBufferSize, lights);

    for(uint32_t i = 0; i < gridSize; i++) {
        grids[i].init();
    }
}