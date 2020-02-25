#include "RenderingPipeline.h"


#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <avg/Debug.h>
#include <glad/glad.h>
#include <csignal>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#include "System/GridLib.h"
#include "RenderLib.h"
#include "ShaderLib.h"
#include "Framebuffer.h"

void RenderingPipeline::init() {
    RenderLib::init();
    skyColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    voxel = RenderLib::create_voxel();
    quadVAO = RenderLib::create_quad();
    boxShader = ShaderLib::program_create("box");
    voxelProgram = ShaderLib::program_create("voxel");
    skyShader = ShaderLib::program_create("skybox");
    polygonMode = 0;

    positionIndex = glGetUniformLocation(voxelProgram, "index");

    PerformanceStat _stat;
    _stat.bufferSize = 32;
    _stat.time = new float[32];
    _stat.id = 0;
    _stat.index = 0;
    _stat.name = "Merging";
    memset(_stat.time, 0, sizeof(float) * 32);

    profiler.stats[1] = _stat;
    profiler.count++;
    perfStat = &profiler.stats[1];
}

void RenderingPipeline::draw_scene(Framebuffer framebuffer, Scene* scene, glm::vec3 view) {
    assert_msg(scene || scene->grids || scene->lights, "Scene is not initialized, it cannot be used for rendering")

    perfStat->count++;
    RenderLib::update();

    RenderLib::bind_framebuffer(framebuffer.framebuffer);
    RenderLib::update();

    draw_sky();

    if(polygonMode == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for(uint32_t i = 0; i < 1; i++) {
        draw_grid(scene->grids[i].cache[(scene->grids[i].cacheIndex) % CACHE_SIZE], view);
    }

    if(polygonMode == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void RenderingPipeline::draw_sky() {
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    RenderLib::bind_vertex_array(voxel);
    ShaderLib::uniform_vec4(skyShader, "skyColor", &skyColor.x);
    RenderLib::draw_voxel(skyShader, glm::vec3(-100.0f, -100.0f, -100.0f), glm::vec3(200.0f, 200.0f, 200.0f));

    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
}

void RenderingPipeline::draw_grid(Grid<int8_t> grid, glm::vec3 view) {
    uint32_t streak = 0;
    uint32_t streakBegin = 0;

    glUseProgram(voxelProgram);
    glBindVertexArray(voxel);

    for(uint32_t i = 0; i < grid.size * grid.size * grid.size; i++) {
        if(grid.grid[i] > 0/*  &&
            (grid.get(i + grid.size) <= 0 ||
            grid.get(i - grid.size) <= 0 ||
            grid.get(i + (grid.size * grid.size)) <= 0 ||
            grid.get(i - (grid.size * grid.size)) <= 0 ||
            grid.get(i + 1) <= 0 ||
            grid.get(i - 1) <= 0) */) {
            streak++;
        } else {
            if(streak > 0) {
                glUniform1i(positionIndex, streakBegin);
                glDrawArraysInstanced(GL_TRIANGLES, 0, 36, streak);
                streakBegin += streak; streak = 0;
            }
            streakBegin++;
        }
    }
}

void RenderingPipeline::terminate() {

}