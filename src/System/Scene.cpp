#include "Scene.h"

#include "Rendering/RenderLib.h"
#include <glad/glad.h>

void Scene::init(uint32_t gridCount) {
    this->_grids = new _Grid[gridCount];
    this->transforms = new glm::mat4[gridCount];
    this->names = new char*[gridCount];
    this->gridsCount = 0;
    this->gridsBufferSize = gridCount;

    lightBufferSize = INITIAL_LIGHT_COUNT;
    lightCount = 0;
    lights = new Light[lightBufferSize];

    lightBuffer = RenderLib::create_buffer_dynamic(UNIFORM_BUFFER, sizeof(Light) * lightBufferSize + sizeof(glm::vec4), lights);
    RenderLib::buffer_binding_range(lightBuffer, 2, 0, sizeof(Light) * lightBufferSize + sizeof(glm::vec4));

    palette = (RGB32*)malloc(sizeof(RGB32) * 256);
    memset(palette, 0, sizeof(RGB32) * 256);
    paletteTexture = TextureLib::create_texture_1d(256, GL_RGB, GL_RGB, palette);
    paletteTexture = paletteTexture;
}

void Scene::add_grid(_Grid grid, char* name) {
    _grids[gridsCount] = grid;
    transforms[gridsCount] = glm::mat4(1.0f);
    names[gridsCount++] = name;
}