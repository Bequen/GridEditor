#include "RenderLib.h"

#include <iostream>
#include <avg/Debug.h>

#include "ShaderLib.h"

void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}

void RenderLib::polygon_mode(uint32_t mode) {
    glPolygonMode(GL_FRONT_AND_BACK, mode);
}

void RenderLib::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    RenderLib::culling(GL_BACK);
}

void RenderLib::update() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
}



#pragma region CREATE PRIMITIVES
uint32_t RenderLib::create_voxel() {
    uint32_t result;
    glGenVertexArrays(1, &result);
    glBindVertexArray(result);
    
    uint32_t VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    #pragma region DATA
    float vertices[] = { 
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,  
        0.5f, -0.5f, 0.5f,  
        0.5f, -0.5f, -0.5f, 
        0.5f, 0.5f, 0.5f,   
        -0.5f, -0.5f, 0.5f, 
        0.5f, -0.5f, 0.5f,  
        0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, 0.5f, 
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f, 
        0.5f, 0.5f, 0.5f,   
        0.5f, 0.5f, -0.5f,  
        -0.5f, 0.5f, 0.5f,  
        -0.5f, 0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f, 
        0.5f, 0.5f, -0.5f,  
        0.5f, -0.5f, -0.5f, 
        0.5f, 0.5f, -0.5f,  
        0.5f, 0.5f, 0.5f,  
        0.5f, -0.5f, 0.5f, 
        0.5f, 0.5f, 0.5f,  
        -0.5f, 0.5f, 0.5f, 
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f, 
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, 0.5f, 
        0.5f, 0.5f, 0.5f
    };

    float normals[] = {
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f
    };
    #pragma endregion

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(vertices)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return result;
}

void RenderLib::delete_vertex_array(uint32_t vao) {
    glDeleteVertexArrays(1, &vao);
}

uint32_t RenderLib::create_quad() {
    uint32_t result;
    glGenVertexArrays(1, &result);
    glBindVertexArray(result);
    
    uint32_t VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    #pragma region DATA
    float vertices[] = { 
        0.0f,   1.0f,   0.0f,
        0.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        1.0f,   1.0f,   0.0f,

        0.0f,   0.0f,   1.0f,
        1.0f,   0.0f,   0.0f,
        0.0f,   0.0f,   0.0f,
        0.0f,   0.0f,   1.0f,
        1.0f,   0.0f,   1.0f,
        1.0f,   0.0f,   0.0f,

        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   0.0f,   1.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   1.0f,


        // Opposite direction
        0.0f,   1.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        0.0f,   0.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        1.0f,   1.0f,   0.0f,
        1.0f,   0.0f,   0.0f,

        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        0.0f,   0.0f,   1.0f,
        1.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   1.0f,

        0.0f,   0.0f,   1.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   0.0f,   0.0f,
        0.0f,   0.0f,   1.0f,
        0.0f,   1.0f,   1.0f,
        0.0f,   1.0f,   0.0f,
    };

    float normals[] = {
        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   1.0f,

        0.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   0.0f,

        1.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,



        0.0f,   0.0f,   -1.0f,
        0.0f,   0.0f,   -1.0f,
        0.0f,   0.0f,   -1.0f,
        0.0f,   0.0f,   -1.0f,
        0.0f,   0.0f,   -1.0f,
        0.0f,   0.0f,   -1.0f,

        0.0f,   -1.0f,   0.0f,
        0.0f,   -1.0f,   0.0f,
        0.0f,   -1.0f,   0.0f,
        0.0f,   -1.0f,   0.0f,
        0.0f,   -1.0f,   0.0f,
        0.0f,   -1.0f,   0.0f,

        -1.0f,   0.0f,   0.0f,
        -1.0f,   0.0f,   0.0f,
        -1.0f,   0.0f,   0.0f,
        -1.0f,   0.0f,   0.0f,
        -1.0f,   0.0f,   0.0f,
        -1.0f,   0.0f,   0.0f,
    };
    #pragma endregion

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(normals), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(normals), normals);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(vertices)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    return result;
}

uint32_t RenderLib::create_render_quad() {
    uint32_t result;
    glGenVertexArrays(1, &result);
    glBindVertexArray(result);
    
    uint32_t VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    #pragma region DATA
    float vertices[] = { 
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
    };

    #pragma endregion

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return result;
}
#pragma endregion

void RenderLib::bind_vertex_array(uint32_t VAO) {
    glBindVertexArray(VAO);
} void RenderLib::bind_buffer(uint32_t target, uint32_t buffer) {
    glBindBuffer(target, buffer);
}

void RenderLib::draw_triangles(uint32_t triangles) {
    glDrawArrays(GL_TRIANGLES, 0, triangles);
} void RenderLib::draw_triangle_strip(uint32_t triangles) {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, triangles);
}



#pragma region BUFFER CREATION
uint32_t RenderLib::create_buffer_stream(uint32_t target, uint32_t size, void* data) {
    uint32_t result;

    glGenBuffers(1, &result);
    glBindBuffer(GL_UNIFORM_BUFFER, result);
    glBufferStorage(GL_UNIFORM_BUFFER, size, data, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    return result;
}

uint32_t RenderLib::create_buffer_dynamic(uint32_t target, uint32_t size, void* data) {
    uint32_t result;

    glGenBuffers(1, &result);
    glBindBuffer(GL_UNIFORM_BUFFER, result);
    glBufferStorage(GL_UNIFORM_BUFFER, size, data, GL_MAP_WRITE_BIT);

    return result;
}


void RenderLib::remove_buffer(uint32_t buffer) {
    glDeleteBuffers(1, &buffer);
}
#pragma endregion



#pragma region BUFFER MAPPING
void* RenderLib::map_buffer_range(uint32_t buffer, uint32_t target, uint32_t offset, uint32_t size) {
    glBindBuffer(target, buffer);
    void* result = glMapBufferRange(target, offset, size, GL_MAP_WRITE_BIT);
    return result;
}

void* RenderLib::map_buffer_stream_range(uint32_t buffer, uint32_t target, uint32_t offset, uint32_t size) {
    glBindBuffer(target, buffer);
    void* result = glMapBufferRange(target, offset, size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    return result;
}

void RenderLib::unmap_buffer(uint32_t target) {
    glUnmapBuffer(target);
}
#pragma endregion



void RenderLib::buffer_binding_range(uint32_t buffer, uint32_t binding, uint32_t offset, uint32_t size) {
    glBindBufferRange(GL_UNIFORM_BUFFER, binding, buffer, offset, size);
}

void RenderLib::draw_sky(RenderInfo renderInfo, uint32_t mode) {
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    glm::vec4 skyColor = glm::vec4(0.529411765f, 0.807843137f, 0.921568627f, 1.0f);

    // If we are drawing in perspective mode
    if(mode == 0) {
        RenderLib::bind_vertex_array(renderInfo.voxelVAO);
        ShaderLib::uniform_vec4(renderInfo.skyProgram, "skyColor", &skyColor.x);
        RenderLib::draw_voxel(renderInfo.skyProgram, glm::vec3(-100.0f, -100.0f, -100.0f), glm::vec3(200.0f, 200.0f, 200.0f));
    } else {
        glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
}

void RenderLib::draw_grid(RenderInfo renderInfo, Grid grid, Transform transform) {
    uint32_t streak = 0;
    uint32_t streakBegin = 0;

    glUseProgram(renderInfo.voxelProgram);
    
    glBindVertexArray(renderInfo.voxelVAO);
    glUniform3ui(glGetUniformLocation(renderInfo.voxelProgram, "size"), grid.width, grid.depth, grid.height);
    glUniformMatrix4fv(glGetUniformLocation(renderInfo.voxelProgram, "model"), 1, GL_FALSE, &transform.transform[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, grid.gridTexture);

    uint32_t positionUniform = glGetUniformLocation(renderInfo.voxelProgram, "index");

    for(uint32_t i = 0; i < grid.width * grid.depth * grid.height; i++) {
        if(grid.get(i) > 0) {
            streak++;
        } else {
            if(streak > 0) {
                glUniform1i(positionUniform, streakBegin);
                glDrawArraysInstanced(GL_TRIANGLES, 0, 36, streak);
                streakBegin += streak; streak = 0;
            }
            streakBegin++;
        }
    }
}


#pragma region Voxel Drawing
void RenderLib::draw_voxel(uint32_t program, float x, float y, float z) {
    glUseProgram(program);

    glm::vec3 position = glm::vec3(x, y, z);
    ShaderLib::uniform_vec3(program, "position", &position[0]);

    glDrawArrays(GL_TRIANGLES, 0, 36);
} void RenderLib::draw_voxel(uint32_t program, glm::vec3 position) {
    glUseProgram(program);

    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

    ShaderLib::uniform_vec3(program, "position", &position[0]);
    ShaderLib::uniform_vec3(program, "scale", &scale[0]);
    ShaderLib::uniform_mat4(program, "model", &model[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 36);
} void RenderLib::draw_voxel(uint32_t program, glm::vec3 position, glm::vec3 scale) {
    glUseProgram(program);

    glm::mat4 model = glm::mat4(1.0f);
    ShaderLib::uniform_vec3(program, "position", &position[0]);
    ShaderLib::uniform_vec3(program, "scale", &scale[0]);
    ShaderLib::uniform_mat4(program, "model", &model[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 36);
} void RenderLib::draw_voxel(uint32_t program, glm::mat4 model, glm::vec3 gridScale) {
    glUseProgram(program);

    glm::vec3 position = glm::vec3(0.0f);
    ShaderLib::uniform_vec3(program, "position", &position[0]);
    ShaderLib::uniform_vec3(program, "scale", &gridScale[0]);
    ShaderLib::uniform_mat4(program, "model", &model[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}

#pragma endregion


void RenderLib::culling(uint32_t mode) {
    glCullFace(mode);
} void RenderLib::front_face(uint32_t face) {
    glFrontFace(face);
}