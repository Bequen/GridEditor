#include "RenderingPipeline.h"

#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <avg/Debug.h>
#include <glad/glad.h>
#include <csignal>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

#include "RenderLib.h"
#include "ShaderLib.h"
#include "Framebuffer.h"

void RenderingPipeline::init() {
    RenderLib::init();
    skyColor = glm::vec4(0.529411765f, 0.807843137f, 0.921568627f, 1.0f);

    renderInfo.voxelVAO = RenderLib::create_voxel();
    renderInfo.quadVAO = RenderLib::create_quad();
    renderInfo.boxProgram = ShaderLib::program_create("box");
    renderInfo.voxelProgram = ShaderLib::program_create("voxel");
    renderInfo.skyProgram = ShaderLib::program_create("skybox");
    polygonMode = 0;

    positionIndex = glGetUniformLocation(renderInfo.voxelProgram, "index");
}

void RenderingPipeline::update() {
    
}

void RenderingPipeline::draw_scene(Framebuffer framebuffer, Scene* scene) {
    RenderLib::update();

    RenderLib::bind_framebuffer(framebuffer.framebuffer);
    RenderLib::update();
}

void RenderingPipeline::draw_sky(uint32_t mode) {
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    // If we are drawing in perspective mode
    if(mode == 0) {
        RenderLib::bind_vertex_array(renderInfo.skyProgram);
        ShaderLib::uniform_vec4(renderInfo.skyProgram, "skyColor", &skyColor.x);
        RenderLib::draw_voxel(renderInfo.skyProgram, glm::vec3(-100.0f, -100.0f, -100.0f), glm::vec3(200.0f, 200.0f, 200.0f));
    } else {
        glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
}

void RenderingPipeline::draw_grid(Grid grid, glm::mat4 model) {
    uint32_t streak = 0;
    uint32_t streakBegin = 0;

    glUseProgram(renderInfo.voxelProgram);
    
    glBindVertexArray(renderInfo.voxelVAO);
    glUniform3ui(glGetUniformLocation(renderInfo.voxelProgram, "size"), grid.width, grid.depth, grid.height);
    glUniformMatrix4fv(glGetUniformLocation(renderInfo.voxelProgram, "model"), 1, GL_FALSE, &model[0][0]);

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