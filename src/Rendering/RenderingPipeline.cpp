#include "RenderingPipeline.h"

#include "RenderLib.h"
#include "ShaderLib.h"

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <avg/Debug.h>
#include <glad/glad.h>
#include <csignal>
#include "QuadBuffer.h"
#include "Quad.h"

void RenderingPipeline::init() {
    RenderLib::init();
    skyColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    voxel = RenderLib::create_voxel();
    topQuadVAO = RenderLib::create_quad();
    shader = ShaderLib::program_create("quad");
    boxShader = ShaderLib::program_create("box");
    skyShader = ShaderLib::program_create("skybox");
    quadProgram = shader;

    ShaderLib::program_use(shader);

    polygonMode = 0;
}

void RenderingPipeline::draw_scene(Scene scene) {
    assert_msg(scene.grids, "Scene Grids are not initialized");
    assert_msg(scene.lights, "Scene Lights are not initialized");

    glDepthMask(GL_FALSE);
    ShaderLib::uniform_vec4(skyShader, "skyColor", &skyColor.x);
    RenderLib::draw_voxel(skyShader, glm::vec3(-100.0f, -100.0f, -100.0f), glm::vec3(200.0f, 200.0f, 200.0f));
    glDepthMask(GL_TRUE);

    RenderLib::culling(GL_BACK);
    glDisable(GL_CULL_FACE);

    RenderLib::bind_vertex_array(topQuadVAO);
    ShaderLib::program_use(shader);

    if(polygonMode == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for(uint32_t i = 0; i < 1; i++) {
        ShaderLib::uniform_int32(shader, "grid", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, scene.grids[i].gridTexture);
        draw_grid(scene.grids[i].cache[scene.grids[i].cacheIndex]);
    }

    if(polygonMode == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_CULL_FACE);
    RenderLib::bind_vertex_array(voxel);

    RenderLib::culling(GL_FRONT);
    RenderLib::draw_voxel(boxShader, glm::vec3((float)0, (float)0, (float)0), glm::vec3(32, 32, 32));
}

void RenderingPipeline::draw_grid(Grid<int8_t> grid) {
    for(uint32_t z = 0; z < grid.size; z++) {
        QuadBuffer buffers[3] = { QuadBuffer(32), QuadBuffer(32), QuadBuffer(32) };

        for(uint32_t y = 0; y < grid.size; y++) {
            for(uint32_t p = 0; p < 3; p++) {
                for(uint32_t i = 0; i < 2; i++) {
                    buffers[p].streak[i] = false;
                    buffers[p].quadIndex[i] = 0;

                    buffers[p].quads[i][y] = new Quad[grid.size];
                    buffers[p].counts[i][y] = 0;
                }
            }

            float dirs[2] = {1.0f, -1.0f};

            for(uint32_t x = 0; x < grid.size; x++) {
                for(uint32_t p = 0; p < 3; p++) {
                    for(uint32_t i = 0; i < 2; i++) {
                        int32_t voxel = grid.get(RenderLib::get_voxel(p, x, y, z));
                        int32_t adjacentVoxel = grid.get(RenderLib::get_adjacent_voxel(p, x, y, z, dirs[i]));
                        if(!buffers[p].streak[i]) {
                            if(voxel > 0 && adjacentVoxel <= 0) {
                                buffers[p].streak[i] = true;
                                buffers[p].quads[i][y][buffers[p].quadIndex[i]] = Quad(x, y, z, y + 1, voxel);
                            }
                        } else {
                            if(voxel != buffers[p].quads[i][y][buffers[p].quadIndex[i]].brush || adjacentVoxel > 0) {
                                buffers[p].streak[i] = false;

                                buffers[p].quads[i][y][buffers[p].quadIndex[i]].w = x;
                                buffers[p].counts[i][y]++;
                                buffers[p].quadIndex[i]++;

                                if(voxel > 0 && adjacentVoxel <= 0) { 
                                    buffers[p].streak[i] = true;
                                    buffers[p].quads[i][y][buffers[p].quadIndex[i]] = Quad(x, y, z, y + 1, voxel);
                                }
                            }
                        }
                    }
                }
            }

            for(uint32_t p = 0; p < 3; p++) {
                for(uint32_t i = 0; i < 2; i++) {
                    if(buffers[p].streak[i]) {
                        buffers[p].streak[i] = false;

                        buffers[p].quads[i][y][buffers[p].quadIndex[i]].w = grid.size;
                        buffers[p].counts[i][y]++;
                        buffers[p].quadIndex[i]++;
                    }
                }
            }
        }

        for(uint32_t p = 0; p < 3; p++) {
            for(uint32_t i = 0; i < 2; i++) {
                solve_greedy_meshing(buffers[p].quads[i], buffers[p].counts[i], 32);
            }
        }

        for(uint32_t p = 0; p < 3; p++) {
            for(uint32_t i = 0; i < 2; i++) {
                for(uint32_t y = 0; y < grid.size; y++) {
                    for(uint32_t x = 0; x < buffers[p].counts[i][y]; x++) {
                        if(buffers[p].quads[i][y][x].d > 0)
                            RenderLib::draw_quad(buffers[p].quads[i][y][x], p, i);
                    }
                    delete [] buffers[p].quads[i][y];
                }
                delete [] buffers[p].counts[i];
            }
        }
    }
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
    /* RenderLib::bind_vertex_array(topQuadVAO);
    ShaderLib::program_use(quadProgram);
    if(polygonMode == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    #pragma region Greedy Meshing
    for(uint32_t z = 0; z < grid->size; z++) {
        QuadBuffer buffers[3] = { QuadBuffer(32), QuadBuffer(32), QuadBuffer(32) };

        for(uint32_t y = 0; y < grid->size; y++) {
            for(uint32_t p = 0; p < 3; p++) {
                for(uint32_t i = 0; i < 2; i++) {
                    buffers[p].streak[i] = false;
                    buffers[p].quadIndex[i] = 0;

                    buffers[p].quads[i][y] = new Quad[grid->size];
                    buffers[p].counts[i][y] = 0;
                }
            }

            float dirs[2] = {1.0f, -1.0f};

            for(uint32_t x = 0; x < grid->size; x++) {
                for(uint32_t p = 0; p < 3; p++) {
                    for(uint32_t i = 0; i < 2; i++) {
                        int32_t voxel = grid->get(RenderLib::get_voxel(p, x, y, z));
                        int32_t adjacentVoxel = grid->get(RenderLib::get_adjacent_voxel(p, x, y, z, dirs[i]));
                        if(!buffers[p].streak[i]) {
                            if(voxel > 0 && adjacentVoxel <= 0) {
                                buffers[p].streak[i] = true;
                                buffers[p].quads[i][y][buffers[p].quadIndex[i]] = Quad(x, y, z, y + 1, voxel);
                            }
                        } else {
                            if(voxel != buffers[p].quads[i][y][buffers[p].quadIndex[i]].brush || adjacentVoxel > 0) {
                                buffers[p].streak[i] = false;

                                buffers[p].quads[i][y][buffers[p].quadIndex[i]].w = x;
                                buffers[p].counts[i][y]++;
                                buffers[p].quadIndex[i]++;

                                if(voxel > 0 && adjacentVoxel <= 0) { 
                                    buffers[p].streak[i] = true;
                                    buffers[p].quads[i][y][buffers[p].quadIndex[i]] = Quad(x, y, z, y + 1, voxel);
                                }
                            }
                        }
                    }
                }
            }

            for(uint32_t p = 0; p < 3; p++) {
                for(uint32_t i = 0; i < 2; i++) {
                    if(buffers[p].streak[i]) {
                        buffers[p].streak[i] = false;

                        buffers[p].quads[i][y][buffers[p].quadIndex[i]].w = grid->size;
                        buffers[p].counts[i][y]++;
                        buffers[p].quadIndex[i]++;
                    }
                }
            }
        }

        for(uint32_t p = 0; p < 3; p++) {
            for(uint32_t i = 0; i < 2; i++) {
                solve_greedy_meshing(buffers[p].quads[i], buffers[p].counts[i], 32);
            }
        }

        for(uint32_t p = 0; p < 3; p++) {
            for(uint32_t i = 0; i < 2; i++) {
                for(uint32_t y = 0; y < grid->size; y++) {
                    for(uint32_t x = 0; x < buffers[p].counts[i][y]; x++) {
                        if(buffers[p].quads[i][y][x].d > 0)
                            RenderLib::draw_quad(buffers[p].quads[i][y][x], p, i);
                    }
                    delete [] buffers[p].quads[i][y];
                }
                delete [] buffers[p].counts[i];
            }
        }
    }
    #pragma endregion

    if(polygonMode == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_CULL_FACE);
    RenderLib::bind_vertex_array(voxel);

    RenderLib::culling(GL_FRONT);
    RenderLib::draw_voxel(boxShader, glm::vec3((float)0, (float)0, (float)0), glm::vec3(grid->size, grid->size, grid->size)); */
}

void RenderingPipeline::solve_greedy_meshing(Quad**& quads, uint32_t*& counts, uint32_t size) {
    for(uint32_t y = 1; y < size; y++) {
        uint32_t x = 0;
        uint32_t merged = 0;
        for(; x < counts[y]; x++) {
            // Runs through the previous line
            for(uint32_t i = merged; i < counts[y - 1]; i++) {
                // If the quads can be merged
                if(quads[y - 1][i].x >= quads[y][x].x &&
                    quads[y - 1][i].w <= quads[y][x].w &&
                    quads[y - 1][i].brush == quads[y][x].brush) {

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

void RenderingPipeline::terminate() {

}