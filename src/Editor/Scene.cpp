#include "Scene.h"

#include "Rendering/RenderLib.h"
#include "Rendering/TextureLib.h"

#include <cstring>
#include <avg/Debug.h>

void Scene::init(uint32_t gridCount) {
    #pragma region Grids
    gridBufferSize = 8;
    grids = new Grid[gridBufferSize];
    gridCount = 0;
    #pragma endregion

    #pragma region Lights
    lightBufferSize = INITIAL_LIGHT_COUNT;
    lightCount = 0;
    lights = new Light[lightBufferSize];

    lightBuffer = RenderLib::create_buffer_dynamic(UNIFORM_BUFFER, sizeof(Light) * lightBufferSize + sizeof(glm::vec4), lights);
    RenderLib::buffer_binding_range(lightBuffer, 2, 0, sizeof(Light) * lightBufferSize + sizeof(glm::vec4));
    #pragma endregion

    #pragma region Palette
    palette = (RGB32*)malloc(sizeof(RGB32) * 256);
    memset(palette, 0, sizeof(RGB32) * 256);
    paletteTexture = TextureLib::create_texture_1d(256, GL_RGB, GL_RGB, palette);
    paletteTexture = paletteTexture;
    #pragma endregion

    sceneGraph = SceneObject(OBJECT_TYPE_EMPTY, nullptr);
    sceneGraph.name = "Scene";

    selected = nullptr;
}

Grid* Scene::add_grid(Grid grid) {
    assert_msg(grids != nullptr, "Grid array was not initialized");

    // If the resize is needed
    if(gridBufferSize == gridCount) {
        gridBufferSize += 8;
        Grid* tempBuffer = new Grid[gridBufferSize];
        if(grids != nullptr) {
            memcpy(tempBuffer, grids, gridCount * sizeof(Grid));
            delete [] grids;
        }

        grids = tempBuffer;
    }

    grids[gridCount] = grid;
    MESSAGE("Adding grid at " << gridCount);
    return &grids[gridCount++];
}

Light* Scene::add_light(Light light) {
    // If the resize is needed
    if(lightBufferSize == lightCount) {
        lightBufferSize += 8;
        Light* tempBuffer = new Light[lightBufferSize];
        if(lights != nullptr) {
            memcpy(tempBuffer, lights, lightCount * sizeof(Light));
            delete [] lights;
        }

        lights = tempBuffer;
    }

    lights[lightCount] = light;

    return &lights[lightCount++];
}

void Scene::update_lights() {
    MESSAGE("Updating lights");
    void* pointer = RenderLib::map_buffer_range(lightBuffer, GL_UNIFORM_BUFFER, 0, sizeof(Light) * MAX_LIGHT_COUNT);
    memcpy(pointer, lights, sizeof(Light) * MAX_LIGHT_COUNT);

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER); 
}

void Scene::assign_grid(SceneObject* sceneObject, uint32_t gridID) {
    sceneObject->data = &grids[gridID];
    sceneObject->type = SCENE_GRID;
}

void Scene::assign_light(SceneObject* sceneObject, uint32_t lightID) {
    sceneObject->data = &lights[lightID];
    sceneObject->type = SCENE_LIGHT;
}