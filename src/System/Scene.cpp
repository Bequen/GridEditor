#include "Scene.h"

#include "Rendering/RenderLib.h"
#include <glad/glad.h>

void Scene::init(uint32_t gridCount) {
    this->gridCount = 1;
    this->gridSize = gridCount;
    grids = new VoxelGrid[gridCount];

    lightBuffer = RenderLib::create_buffer_dynamic(UNIFORM_BUFFER, sizeof(Light) * lightBufferSize + sizeof(glm::vec4), lights);
    RenderLib::buffer_binding_range(lightBuffer, 2, 0, sizeof(Light) * lightBufferSize + sizeof(glm::vec4));

    for(uint32_t i = 0; i < gridSize; i++) {
        grids[i].init(128);

        grids[i].quadMesh.counts[0] = 0;
    }

    palette = (RGB32*)malloc(sizeof(RGB32) * 256);
    memset(palette, 0, sizeof(RGB32) * 256);
    paletteTexture = TextureLib::create_texture_1d(256, GL_RGB, GL_RGB, palette);
    paletteTexture = paletteTexture;
}