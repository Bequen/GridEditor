#include "RenderingPipeline.h"

#include "RenderLib.h"
#include "ShaderLib.h"

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <avg/Debug.h>
#include <glad/glad.h>
#include <csignal>
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
    // Stupid
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    RenderLib::bind_vertex_array(topQuadVAO);
    ShaderLib::program_use(quadProgram);

    #pragma region Z AXIS 
    for(uint32_t z = 0; z < grid->size; z++) {
        uint32_t streak = false;
        uint32_t quadIndex = 0;
        Quad** quads = new Quad*[grid->size];
        uint32_t* counts = new uint32_t[grid->size];

        uint32_t streak_o = false;
        uint32_t quadIndex_o = 0;
        Quad** quads_o = new Quad*[grid->size];
        uint32_t* counts_o = new uint32_t[grid->size];

        for(uint32_t y = 0; y < grid->size; y++) {
            quadIndex = 0;
            quads[y] = new Quad[grid->size];
            streak = false;
            counts[y] = 0;

            quadIndex_o = 0;
            quads_o[y] = new Quad[grid->size];
            streak_o = false;
            counts_o[y] = 0;

            for(uint32_t x = 0; x < grid->size; x++) {
                if(grid->get(glm::vec3(x, y, z)) > 0 && grid->get(glm::vec3(x, y, z + 1)) <= 0) {
                    if(!streak) {
                        streak = true;
                        quads[y][quadIndex].x = x;
                        quads[y][quadIndex].y = y;
                        quads[y][quadIndex].z = z;
                        quads[y][quadIndex].d = y + 1;
                    }
                } else {
                    if(streak) {
                        streak = false;

                        quads[y][quadIndex].w = x;
                        counts[y]++;
                        quadIndex++;
                    }
                }



                if(grid->get(glm::vec3(x, y, z)) > 0 && grid->get(glm::vec3(x, y, z - 1)) <= 0) {
                    if(!streak_o) {
                        streak_o = true;
                        quads_o[y][quadIndex_o].x = x;
                        quads_o[y][quadIndex_o].y = y;
                        quads_o[y][quadIndex_o].z = z;
                        quads_o[y][quadIndex_o].d = y + 1;
                    }
                } else {
                    if(streak_o) {
                        streak_o = false;

                        quads_o[y][quadIndex_o].w = x;
                        counts_o[y]++;
                        quadIndex_o++;
                    }
                }
            }

            if(streak) {
                streak = false;

                quads[y][quadIndex].w = grid->size;
                counts[y]++;
                quadIndex++;
            } if(streak_o) {
                streak_o = false;

                quads_o[y][quadIndex_o].w = grid->size;
                counts_o[y]++;
                quadIndex_o++;
            }
        }

        solve_greedy_meshing(quads, counts);
        solve_greedy_meshing(quads_o, counts_o);

        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t x = 0; x < counts[y]; x++) {
                if(quads[y][x].d > 0)
                    RenderLib::draw_quad_z(quads[y][x], false);
                
            }
            for(uint32_t x = 0; x < counts_o[y]; x++) {
                if(quads_o[y][x].d > 0)
                    RenderLib::draw_quad_z(quads_o[y][x], true);
            }

            delete [] quads[y];
            delete [] quads_o[y];
        }

        delete [] counts;
        delete [] counts_o;
    }
    #pragma endregion



    #pragma region Y AXIS 
    for(uint32_t z = 0; z < grid->size; z++) {
        uint32_t streak = false;
        uint32_t quadIndex = 0;
        Quad** quads = new Quad*[grid->size];
        uint32_t* counts = new uint32_t[grid->size];

        uint32_t streak_o = false;
        uint32_t quadIndex_o = 0;
        Quad** quads_o = new Quad*[grid->size];
        uint32_t* counts_o = new uint32_t[grid->size];

        for(uint32_t y = 0; y < grid->size; y++) {
            quadIndex = 0;
            quads[y] = new Quad[grid->size];
            streak = false;
            counts[y] = 0;

            quadIndex_o = 0;
            quads_o[y] = new Quad[grid->size];
            streak_o = false;
            counts_o[y] = 0;

            for(uint32_t x = 0; x < grid->size; x++) {
                if(grid->get(glm::vec3(x, z, y)) > 0 && grid->get(glm::vec3(x, z + 1, y)) <= 0) {
                    if(!streak) {
                        streak = true;
                        quads[y][quadIndex].x = x;
                        quads[y][quadIndex].y = y;
                        quads[y][quadIndex].z = z;
                        quads[y][quadIndex].d = y + 1;
                    }
                } else {
                    if(streak) {
                        streak = false;

                        quads[y][quadIndex].w = x;
                        counts[y]++;
                        quadIndex++;
                    }
                }



                if(grid->get(glm::vec3(x, z, y)) > 0 && grid->get(glm::vec3(x, z - 1, y)) <= 0) {
                    if(!streak_o) {
                        streak_o = true;
                        quads_o[y][quadIndex_o].x = x;
                        quads_o[y][quadIndex_o].y = y;
                        quads_o[y][quadIndex_o].z = z;
                        quads_o[y][quadIndex_o].d = y + 1;
                    }
                } else {
                    if(streak_o) {
                        streak_o = false;

                        quads_o[y][quadIndex_o].w = x;
                        counts_o[y]++;
                        quadIndex_o++;
                    }
                }
            }

            if(streak) {
                streak = false;

                quads[y][quadIndex].w = grid->size;
                counts[y]++;
                quadIndex++;
            } if(streak_o) {
                streak_o = false;

                quads_o[y][quadIndex_o].w = grid->size;
                counts_o[y]++;
                quadIndex_o++;
            }
        }

        solve_greedy_meshing(quads, counts);
        solve_greedy_meshing(quads_o, counts_o);

        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t x = 0; x < counts[y]; x++) {
                if(quads[y][x].d > 0)
                    RenderLib::draw_quad_y(quads[y][x], false);
                
            }
            for(uint32_t x = 0; x < counts_o[y]; x++) {
                if(quads_o[y][x].d > 0)
                    RenderLib::draw_quad_y(quads_o[y][x], true);
            }

            delete [] quads[y];
            delete [] quads_o[y];
        }

        delete [] counts;
        delete [] counts_o;
    }
    #pragma endregion



    #pragma region X AXIS 
    for(uint32_t z = 0; z < grid->size; z++) {
        uint32_t streak = false;
        uint32_t quadIndex = 0;
        Quad** quads = new Quad*[grid->size];
        uint32_t* counts = new uint32_t[grid->size];

        uint32_t streak_o = false;
        uint32_t quadIndex_o = 0;
        Quad** quads_o = new Quad*[grid->size];
        uint32_t* counts_o = new uint32_t[grid->size];

        for(uint32_t y = 0; y < grid->size; y++) {
            quadIndex = 0;
            quads[y] = new Quad[grid->size + 1];
            streak = false;
            counts[y] = 0;

            quadIndex_o = 0;
            quads_o[y] = new Quad[grid->size + 1];
            streak_o = false;
            counts_o[y] = 0;

            for(uint32_t x = 0; x < grid->size; x++) {
                if(grid->get(glm::vec3(z, x, y)) > 0 && grid->get(glm::vec3(z + 1, x, y)) <= 0) {
                    if(!streak) {
                        streak = true;
                        quads[y][quadIndex].x = x;
                        quads[y][quadIndex].y = y;
                        quads[y][quadIndex].z = z;
                        quads[y][quadIndex].d = y + 1;
                    }
                } else {
                    if(streak) {
                        streak = false;

                        quads[y][quadIndex].w = x;
                        counts[y]++;
                        quadIndex++;
                    }
                }



                if(grid->get(glm::vec3(z, x, y)) > 0 && grid->get(glm::vec3(z - 1, x, y)) <= 0) {
                    if(!streak_o) {
                        streak_o = true;
                        quads_o[y][quadIndex_o].x = x;
                        quads_o[y][quadIndex_o].y = y;
                        quads_o[y][quadIndex_o].z = z;
                        quads_o[y][quadIndex_o].d = y + 1;
                    }
                } else {
                    if(streak_o) {
                        streak_o = false;

                        quads_o[y][quadIndex_o].w = x;
                        counts_o[y]++;
                        quadIndex_o++;
                    }
                }
            }

            if(streak) {
                streak = false;

                quads[y][quadIndex].w = grid->size;
                counts[y]++;
                quadIndex++;
            } if(streak_o) {
                streak_o = false;

                quads_o[y][quadIndex_o].w = grid->size;
                counts_o[y]++;
                quadIndex_o++;
            }
        }

        solve_greedy_meshing(quads, counts);
        solve_greedy_meshing(quads_o, counts_o);

        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t x = 0; x < counts[y]; x++) {
                if(quads[y][x].d > 0)
                    RenderLib::draw_quad_x(quads[y][x], false);
                
            }
            for(uint32_t x = 0; x < counts_o[y]; x++) {
                if(quads_o[y][x].d > 0)
                    RenderLib::draw_quad_x(quads_o[y][x], true);
            }

            free(quads[y]);
            free(quads_o[y]);
        }

        delete [] counts;
        delete [] counts_o;
    }
    #pragma endregion

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    RenderLib::bind_vertex_array(voxel);

    RenderLib::culling(GL_FRONT);
    RenderLib::draw_voxel(boxShader, glm::vec3((float)0, (float)0, (float)0), glm::vec3(grid->size, grid->size, grid->size));
}

void RenderingPipeline::solve_greedy_meshing(Quad**& quads, uint32_t*& counts) {
    ERROR("PICO BEGIN");
    for(uint32_t y = 1; y < grid->size; y++) {
        uint32_t x = 0;
        uint32_t merged = 0;
        for(; x < counts[y]; x++) {
            // Runs through the previous line
            for(uint32_t i = merged; i < counts[y - 1]; i++) {
                // If the quads can be merged
                if(quads[y - 1][i].x >= quads[y][x].x &&
                    quads[y - 1][i].w <= quads[y][x].w) {

                    Quad previous = quads[y - 1][i];
                    Quad current = quads[y][x];
                    uint32_t index = 0;
                    uint32_t offset = 0;

                    uint32_t quadIndex = x;
                    uint32_t splitIndex = counts[y];

                    // If the previous quad is already merged
                    if(previous.d == 0) {
                        // Use the index of the first quad
                        index = previous.y;
                        // Also save the offset
                        offset = previous.z;
                    } else {
                        index = y - 1;
                        offset = i;
                    }

                    if(index == current.y) {
                        continue;
                    }
                    // Adds height to the quad
                    quads[index][offset].d = y + 1;

                    // Although the quad splits, create two new quads and copy data into them
                    memcpy(&quads[y][splitIndex], &quads[y][quadIndex], sizeof(Quad));
                    memcpy(&quads[y][splitIndex + 1], &quads[y][quadIndex], sizeof(Quad));
                    counts[y] += 2;

                    // Edit the first quad so it ends correctly
                    quads[y][quadIndex].w = quads[index][offset].x;

                    // Change the next face to the merged state
                    quads[y][splitIndex].x = quads[index][offset].x;
                    quads[y][splitIndex].w = quads[index][offset].w;

                    quads[y][splitIndex].d = 0;
                    quads[y][splitIndex].y = index;
                    quads[y][splitIndex].z = offset;

                    // Save some data
                    if(quads[y][quadIndex].w == quads[y][quadIndex].x) {
                        memcpy(&quads[y][quadIndex], &quads[y][splitIndex], sizeof(Quad));
                        memcpy(&quads[y][splitIndex], &quads[y][splitIndex + 1], sizeof(Quad));
                        splitIndex--;
                        counts[y]--;
                        merged--;
                    }

                    splitIndex++;

                    // The last split quad starts at the end of the merged quad
                    quads[y][splitIndex].x = quads[index][offset].w;

                    merged++;
                    // If the overral width is over 0, means the face truly exist, so add it
                    if(quads[y][splitIndex].x == quads[y][splitIndex].w) {
                        counts[y]--;
                        merged--;
                    }

                    merged++;
                    break;
                }
            }
        }
    }
}

void RenderingPipeline::greedy_meshing() {
    RenderLib::bind_vertex_array(topQuadVAO);
    ShaderLib::program_use(quadProgram);

    uint32_t streakZ = 0;

    for(uint32_t z = 0; z < grid->size; z++) {
        Quad** quadsZ = new Quad*[grid->size];
        uint32_t* countsZ = new uint32_t[grid->size];

        for(uint32_t y = 0; y < grid->size; y++) {
            quadsZ[y] = new Quad[grid->size];
            uint32_t quadIndexZ = 0;
            uint32_t mergeIndexZ = 0;
            countsZ[y] = 0;
            streakZ = false;

            for(uint32_t x = 0; x < grid->size; x++) {
                if(grid->get(glm::vec3(x, y, z)) > 0 && grid->get(glm::vec3(x, y, z + 1)) <= 0) {
                    if(!streakZ) {
                        streakZ = true;
                        quadsZ[y][quadIndexZ].x = x;
                        quadsZ[y][quadIndexZ].y = y;
                        quadsZ[y][quadIndexZ].z = z;
                        quadsZ[y][quadIndexZ].h = 1;
                    }
                } else {
                    if(streakZ) {
                        streakZ = false;

                        quadsZ[y][quadIndexZ].w = x;

                        /* if(y > 0) {
                            for(uint32_t i = mergeIndexZ; i < countsZ[y - 1]; i++) {
                                if(quadsZ[y][quadIndexZ].x <= quadsZ[y - 1][i].x &&
                                    quadsZ[y][quadIndexZ].w >= quadsZ[y - 1][i].w) {

                                    Quad previous = quadsZ[y - 1][i];
                                    uint32_t index = 0;
                                    uint32_t offset = 0;

                                    // If the previous quad is already merged
                                    if(previous.h == 0) {
                                        // Use the index of the first quad
                                        index = previous.y;
                                        // Also save the offset
                                        offset = previous.z;
                                    } else {
                                        index = y - 1;
                                        offset = i;
                                    }
                                    // Adds height to the quad
                                    quadsZ[index][offset].h++;

                                    // Although the quad splits, create two new quads and copy data into them
                                    memcpy(&quadsZ[y][quadIndexZ + 1], &quadsZ[y][quadIndexZ], sizeof(Quad));
                                    memcpy(&quadsZ[y][quadIndexZ + 2], &quadsZ[y][quadIndexZ], sizeof(Quad));

                                    // Edit the first quad so it ends correctly
                                    quadsZ[y][quadIndexZ].w = quadsZ[index][offset].x;
                                    if(quadsZ[y][quadIndexZ].w == quadsZ[y][quadIndexZ].x)
                                        quadIndexZ--;

                                    // Change the next face to the merged state
                                    quadsZ[y][quadIndexZ + 1].x = quadsZ[index][offset].x;
                                    quadsZ[y][quadIndexZ + 1].w = quadsZ[index][offset].w;

                                    quadsZ[y][quadIndexZ + 1].h = 0;
                                    quadsZ[y][quadIndexZ + 1].y = index;
                                    quadsZ[y][quadIndexZ + 1].z = offset;

                                    countsZ[y]++;
                                    quadIndexZ++;

                                    // The last split quad starts at the end of the merged quad
                                    quadsZ[y][quadIndexZ + 1].x = quadsZ[index][offset].w;

                                    // If the overral width is over 0, means the face truly exist, so add it
                                    if(quadsZ[y][quadIndexZ + 1].x != quadsZ[y][quadIndexZ + 1].w) {
                                        countsZ[y]++;
                                        quadIndexZ++;
                                    }
                                    mergeIndexZ++;
                                    break;
                                }
                            }
                        } */

                        countsZ[y]++;
                        quadIndexZ++;
                    }
                }
            }
        }

        solve_greedy_meshing(quadsZ, countsZ);

        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t x = 0; x < countsZ[y]; x++) {
                if(quadsZ[y][x].h != 0)
                    RenderLib::draw_quad_y(quadsZ[y][x], false);
            }
        }
    }
}

void RenderingPipeline::terminate() {

}