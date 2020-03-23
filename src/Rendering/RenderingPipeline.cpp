#include "RenderingPipeline.h"


#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <avg/Debug.h>
#include <glad/glad.h>
#include <csignal>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#include "System/Voxels/GridLib.h"
#include "RenderLib.h"
#include "ShaderLib.h"
#include "Framebuffer.h"

void RenderingPipeline::init() {
    RenderLib::init();
    skyColor = glm::vec4(0.529411765f, 0.807843137f, 0.921568627f, 1.0f);

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

void RenderingPipeline::draw_scene(Framebuffer framebuffer, Scene* scene) {
    assert_msg(scene || scene->_grids || scene->lights, "Scene is not initialized, it cannot be used for rendering")

    perfStat->count++;
    RenderLib::update();

    RenderLib::bind_framebuffer(framebuffer.framebuffer);
    RenderLib::update();
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

void RenderingPipeline::draw_grid(_Grid grid, glm::mat4 model) {
    uint32_t streak = 0;
    uint32_t streakBegin = 0;

    glUseProgram(voxelProgram);
    
    glBindVertexArray(voxel);
    glUniform3ui(glGetUniformLocation(voxelProgram, "size"), grid.width, grid.depth, grid.height);
    glUniformMatrix4fv(glGetUniformLocation(voxelProgram, "model"), 1, GL_FALSE, &model[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, grid.gridTexture);

    for(uint32_t i = 0; i < grid.width * grid.depth * grid.height; i++) {
        if(grid.get(i) > 0) {
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