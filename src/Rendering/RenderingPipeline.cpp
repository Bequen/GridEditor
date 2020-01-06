#include "RenderingPipeline.h"

#include "RenderLib.h"
#include "ShaderLib.h"

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <avg/Debug.h>
#include <glad/glad.h>

void RenderingPipeline::init(Grid<int8_t>* grid) {
    this->grid = grid;
    RenderLib::init();

    voxel = RenderLib::create_voxel();
    shader = ShaderLib::program_create("default/default");
    boxShader = ShaderLib::program_create("box");
    ShaderLib::program_use(shader);
}

void RenderingPipeline::update() {
    RenderLib::culling(GL_BACK);
    for(uint32_t x = 0; x < grid->size; x++) {
        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t z = 0; z < grid->size; z++) {
                if(grid->get(x, y, z) != 0) {
                    RenderLib::draw_voxel(shader, glm::vec3((float)x, (float)y, (float)z));
                }
            }
        }
    }

    RenderLib::culling(GL_FRONT);
    RenderLib::draw_voxel(boxShader, glm::vec3((float)0, (float)0, (float)0), glm::vec3(grid->size, grid->size, grid->size));
}

void RenderingPipeline::terminate() {

}