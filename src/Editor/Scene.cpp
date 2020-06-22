#include "Scene.h"

#include "Rendering/RenderLib.h"
#include "Rendering/TextureLib.h"

#include <cstring>
#include <avg/Debug.h>

void Scene::init(uint32_t gridCount) {
    #pragma region Grids
    _gridBufferSize = 8;
    _grids = new SceneGrid[_gridBufferSize];
    _gridCount = 0;
    #pragma endregion

    #pragma region Sprites
    spriteBufferSize = 8;
    sprites = new SceneSprite[spriteBufferSize];
    spriteCount = 0;
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

    materials = new Material[256];
    materialsBuffer = RenderLib::create_buffer_dynamic(UNIFORM_BUFFER, sizeof(Material) * 256, materials);
    RenderLib::buffer_binding_range(materialsBuffer, 3, 0, sizeof(Material) * 256);
    #pragma endregion

    sceneGraph = SceneObject(OBJECT_TYPE_EMPTY, nullptr);
    sceneGraph.name = "Scene";

    selected = nullptr;
}

SceneGrid* Scene::add_grid(SceneGrid grid) {
    assert_msg(_grids != nullptr, "Grid array was not initialized");

    // If the resize is needed
    if(_gridBufferSize == _gridCount) {
        _gridBufferSize += 8;
        SceneGrid* tempBuffer = new SceneGrid[_gridBufferSize];
        if(_grids != nullptr) {
            memcpy(tempBuffer, _grids, _gridCount * sizeof(Grid));
            delete [] _grids;
        }

        _grids = tempBuffer;
    }

    _grids[_gridCount] = grid;
    MESSAGE("Adding grid at " << _gridCount);
    return &_grids[_gridCount++];
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

SceneSprite* Scene::add_sprite(SceneSprite sprite) {
    assert_msg(sprites != nullptr, "Sprite array was not initialized");

    // If the resize is needed
    if(spriteBufferSize == spriteCount) {
        spriteBufferSize += 8;
        SceneSprite* tempBuffer = new SceneSprite[spriteBufferSize];
        if(sprites != nullptr) {
            memcpy(tempBuffer, sprites, spriteCount * sizeof(SceneSprite));
            delete [] sprites;
        }

        sprites = tempBuffer;
    }

    sprites[spriteCount] = sprite;
    MESSAGE("Adding sprite at " << spriteCount);
    return &sprites[spriteCount++];
}

void Scene::update_lights() {
    MESSAGE("Updating lights");
    void* pointer = RenderLib::map_buffer_range(lightBuffer, GL_UNIFORM_BUFFER, 0, sizeof(Light) * MAX_LIGHT_COUNT);
    memcpy(pointer, lights, sizeof(Light) * MAX_LIGHT_COUNT);

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER); 
}