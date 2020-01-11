#include "RenderingPipeline.h"

#include "RenderLib.h"
#include "ShaderLib.h"

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <avg/Debug.h>
#include <glad/glad.h>
#include "Quad.h"

void RenderingPipeline::init(Grid<int8_t>* grid) {
    this->grid = grid;
    RenderLib::init();
    skyColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    voxel = RenderLib::create_voxel();
    topQuadVAO = RenderLib::create_quad_top();
    shader = ShaderLib::program_create("default/default");
    boxShader = ShaderLib::program_create("box");
    skyShader = ShaderLib::program_create("skybox");
    quadProgram = ShaderLib::program_create("quad");
    ShaderLib::program_use(shader);
}

void RenderingPipeline::update() {
    glDepthMask(GL_FALSE);
    ShaderLib::uniform_vec4(skyShader, "skyColor", &skyColor.x);
    RenderLib::draw_voxel(skyShader, glm::vec3(-100.0f, -100.0f, -100.0f), glm::vec3(200.0f, 200.0f, 200.0f));
    glDepthMask(GL_TRUE);

    RenderLib::culling(GL_BACK);
    glDisable(GL_CULL_FACE);
    /* for(uint32_t z = 0; z < grid->size; z++) {
        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t x = 0; x < grid->size; x++) {
                if(grid->grid[x + z * (grid->size * grid->size) + y * grid->size] != 0) {
                    RenderLib::draw_voxel(shader, glm::vec3((float)x, (float)y, (float)z));
                }
            }
        }
    } */

    // Greedy
    glm::vec3 position;
    glm::vec3 scale;
    uint32_t canExpand = 1;

    uint32_t streakY = 0;

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    RenderLib::bind_vertex_array(topQuadVAO);
    ShaderLib::program_use(quadProgram);
    /* for(uint32_t y = 0; y < grid->size; y++) {
        Quad** quadsY = new Quad*[grid->size];
        uint32_t* countsY = new uint32_t[grid->size];

        for(uint32_t z = 0; z < grid->size; z++) {
            quadsY[y] = new Quad[grid->size];
            uint32_t quadIndexY = 0;
            uint32_t mergeIndex = 0;
            countsY[y] = 0;

            for(uint32_t x = 0; x < grid->size; x++) {
                if(grid->get(glm::vec3(x, y, z)) > 0 && grid->get(glm::vec3(x, y + 1, z)) <= 0) {
                    if(!streakY) {
                        streakY = true;
                        quadsY[y][quadIndexY].x = x;
                        quadsY[y][quadIndexY].y = y;
                        quadsY[y][quadIndexY].z = z;
                        quadsY[y][quadIndexY].h = 1;
                    }
                } else {
                    if(streakY) {
                        streakY = false;
                        quadsY[y][quadIndexY].w = x;

                        if(z > 0) {
                            ERROR("Merging " << mergeIndex << "|" << countsY[y - 1]);
                            for(uint32_t i = mergeIndex; i < countsY[y - 1]; i++) {
                                if(quadsY[y][quadIndexY].x <= quadsY[y - 1][i].x &&
                                    quadsY[y][quadIndexY].w >= quadsY[y - 1][i].w) {
                                        quadsY[y - 1][i].h++;

                                        if(quadsY[y][quadIndexY].x < quadsY[y - 1][mergeIndex].x) {
                                            quadsY[y][quadIndexY].w = quadsY[y - 1][mergeIndex].x;
                                        } else {
                                            if(quadsY[y][quadIndexY].w > quadsY[y - 1][mergeIndex].w) {
                                                quadsY[y][quadIndexY].x = quadsY[y - 1][mergeIndex].w;
                                                mergeIndex--;
                                            }
                                        }

                                        mergeIndex++;
                                        break;
                                    }
                            }
                        }

                        countsY[y]++;
                        quadIndexY++;
                    }
                }
            }
        }

        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t x = 0; x < countsY[y]; x++) {
                RenderLib::draw_quad_y(quadsY[y][x]);
            }
        }
    }  */

    for(uint32_t z = 0; z < grid->size; z++) {
        Quad** quadsY = new Quad*[grid->size];
        uint32_t* countsY = new uint32_t[grid->size];

        for(uint32_t y = 0; y < grid->size; y++) {
            quadsY[y] = new Quad[grid->size];
            uint32_t quadIndexY = 0;
            uint32_t mergeIndex = 0;
            countsY[y] = 0;

            for(uint32_t x = 0; x < grid->size; x++) {
                if(grid->get(glm::vec3(x, y, z)) > 0 && grid->get(glm::vec3(x, y, z + 1)) <= 0) {
                    if(!streakY) {
                        streakY = true;
                        quadsY[y][quadIndexY].x = x;
                        quadsY[y][quadIndexY].y = y;
                        quadsY[y][quadIndexY].z = z;
                        quadsY[y][quadIndexY].h = 1;
                    }
                } else {
                    if(streakY) {
                        streakY = false;

                        quadsY[y][quadIndexY].w = x;

                        /* if(z > 0) {
                            for(uint32_t i = mergeIndex; i < countsY[z - 1]; i++) {
                                if(quadsY[z][quadIndexY].x <= quadsY[z - 1][i].x &&
                                    quadsY[z][quadIndexY].w >= quadsY[z - 1][i].w) {
                                    ERROR("Merging " << mergeIndex << "|" << countsY[z - 1]);
                                        quadsY[z - 1][i].h++;

                                        if(quadsY[z][quadIndexY].x < quadsY[z - 1][mergeIndex].x) {
                                            quadsY[z][quadIndexY].w = quadsY[z - 1][mergeIndex].x;
                                        } else {
                                            if(quadsY[z][quadIndexY].w > quadsY[z - 1][mergeIndex].w) {
                                                quadsY[z][quadIndexY].x = quadsY[z - 1][mergeIndex].w;
                                                mergeIndex--;
                                            }
                                        }

                                        mergeIndex++;
                                        break;
                                    }
                            }
                        } */

                        countsY[y]++;
                        quadIndexY++;
                    }
                }
            }
        }

        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t x = 0; x < countsY[y]; x++) {
                RenderLib::draw_quad_y(quadsY[y][x]);
                
            }
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    RenderLib::bind_vertex_array(voxel);

    RenderLib::culling(GL_FRONT);
    RenderLib::draw_voxel(boxShader, glm::vec3((float)0, (float)0, (float)0), glm::vec3(grid->size, grid->size, grid->size));
}

void RenderingPipeline::terminate() {

}