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
    glEnable(GL_DEPTH_TEST);

    voxel = RenderLib::create_voxel();
    shader = ShaderLib::program_create("default/default");
    ShaderLib::program_use(shader);
}

void RenderingPipeline::update() {
    for(uint32_t x = 0; x < grid->size; x++) {
        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t z = 0; z < grid->size; z++) {
                if(grid->get(x, y, z) != 0) {
                    RenderLib::draw_voxel(shader, (float)x, (float)y, (float)z);
                }
            }
        }
    }
}

void RenderingPipeline::terminate() {

}