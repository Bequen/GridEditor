#include "RenderingPipeline.h"


#include <cstring>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <avg/Debug.h>
#include <glad/glad.h>
#include <csignal>


#include "System/GridLib.h"
#include "RenderLib.h"
#include "ShaderLib.h"
#include "QuadBuffer.h"
#include "Quad.h"
#include "Framebuffer.h"

void RenderingPipeline::init() {
    RenderLib::init();
    skyColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    voxel = RenderLib::create_voxel();
    quadVAO = RenderLib::create_quad();
    shader = ShaderLib::program_create("quad");
    boxShader = ShaderLib::program_create("box");
    skyShader = ShaderLib::program_create("skybox");
    quadProgram = shader;

    ShaderLib::program_use(shader);

    polygonMode = 0;

    position = glGetUniformLocation(shader, "position");
    scale = glGetUniformLocation(shader, "scale");
}

void RenderingPipeline::draw_scene(Framebuffer framebuffer, Scene* scene) {
    assert_msg(scene || scene->grids || scene->lights, "Scene is not initialized, it cannot be used for rendering")

    RenderLib::update();

    RenderLib::bind_framebuffer(framebuffer.framebuffer);
    RenderLib::update();

    draw_sky();

    RenderLib::bind_vertex_array(quadVAO);
    ShaderLib::program_use(shader);

    if(polygonMode == 1)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for(uint32_t i = 0; i < 1; i++) {
        draw_grid(scene->grids[0].cache[(scene->grids[0].cacheIndex) % CACHE_SIZE]);
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

void RenderingPipeline::draw_grid(Grid<int8_t> grid) {
    for(uint32_t z = 0; z < grid.size; z++) {
        for(uint32_t y = 0; y < grid.size; y++) {
            uint32_t streakX = 0;
            uint32_t startX = 0;

            uint32_t streakY = 0;
            uint32_t startY = 0;
            
            uint32_t _streakX = 0;
            uint32_t _startX = 0;

            uint32_t _streakY = 0;
            uint32_t _startY = 0;
            for(uint32_t x = 0; x < grid.size; x++) {
                if(grid.get(x, y, z) > 0) {
                    if(grid.get(x, y, z + 1) <= 0) {
                        streakX++;
                    } else {
                        if(streakX > 0) {
                            glUniform3f(position, startX, y, z);
                            glUniform3f(scale, streakX, 1, 1);
                            RenderLib::render_quad(0, 0);
                            streakX = 0;
                        }
                        startX = x + 1;
                    }

                    if(grid.get(x, y, z - 1) <= 0) {
                        _streakX++;
                    } else {
                        if(_streakX > 0) {
                            glUniform3f(position, _startX, y, z);
                            glUniform3f(scale, _streakX, 1, 1);
                            RenderLib::render_quad(0, 1);
                            _streakX = 0;
                        }
                        _startX = x + 1;
                    }

                    if(grid.get(x, y + 1, z) <= 0) {
                        streakY++;
                    } else {
                        if(streakY > 0) {
                            glUniform3f(position, startY, y, z);
                            glUniform3f(scale, streakY, 1, 1);
                            RenderLib::render_quad(1, 0);
                            streakY = 0;
                        }
                        startY = x + 1;
                    } if(grid.get(x, y - 1, z) <= 0) {
                        _streakY++;
                    } else {
                        if(_streakY > 0) {
                            glUniform3f(position, _startY, y, z);
                            glUniform3f(scale, _streakY, 1, 1);
                            RenderLib::render_quad(1, 0);
                            _streakY = 0;
                        }
                        _startY = x + 1;
                    }

/*                     glUniform3f(position, x, y, z);
                    glUniform3f(scale, 1, 1, 1); */

                    /* if(grid.get(x, y, z + 1) <= 0)
                        RenderLib::render_quad(0, 0);
                    if(grid.get(x, y, z - 1) <= 0)
                        RenderLib::render_quad(0, 1);

                    if(grid.get(x, y + 1, z) <= 0)
                        RenderLib::render_quad(1, 0);
                    if(grid.get(x, y - 1, z) <= 0)
                        RenderLib::render_quad(1, 1); */

                    if(grid.get(x + 1, y, z ) <= 0) {
                        glUniform3f(position, x, y, z);
                        RenderLib::render_quad(2, 0);
                    } if(grid.get(x - 1, y, z ) <= 0) {
                        glUniform3f(position, x, y, z);
                        RenderLib::render_quad(2, 1);
                    }
                } else {
                    if(streakX > 0) {
                        glUniform3f(position, startX, y, z);
                        glUniform3f(scale, streakX, 1, 1);
                        RenderLib::render_quad(0, 0);
                        streakX = 0;
                    } if(_streakX > 0) {
                        glUniform3f(position, _startX, y, z);
                        glUniform3f(scale, _streakX, 1, 1);
                        RenderLib::render_quad(0, 1);
                        _streakX = 0;
                    }
                    startX = x + 1;
                    _startX = x + 1;

                    if(streakY > 0) {
                        glUniform3f(position, startY, y, z);
                        glUniform3f(scale, streakY, 1, 1);
                        RenderLib::render_quad(1, 0);
                        streakY = 0;
                    } if(_streakY > 0) {
                        glUniform3f(position, _startY, y, z);
                        glUniform3f(scale, _streakY, 1, 1);
                        RenderLib::render_quad(1, 1);
                        _streakY = 0;
                    }
                    startY = x + 1;
                    _startY = x + 1;
                }
            }
        }
    }
}

void RenderingPipeline::terminate() {

}