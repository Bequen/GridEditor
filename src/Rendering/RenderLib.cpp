#include "RenderLib.h"

#include <glad/glad.h>
#include <iostream>
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

void RenderLib::init() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );

    
    RenderLib::culling(GL_BACK);
}

void RenderLib::update() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

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

void RenderLib::bind_vertex_array(uint32_t VAO) {
    glBindVertexArray(VAO);
}

void RenderLib::draw_triangles(uint32_t triangles) {
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

uint32_t RenderLib::create_buffer_stream(uint32_t target, uint32_t size, void* data) {
    uint32_t result;

    glGenBuffers(1, &result);
    glBindBuffer(GL_UNIFORM_BUFFER, result);
    glBufferStorage(GL_UNIFORM_BUFFER, size, data, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    return result;
}

void* RenderLib::map_buffer_stream(uint32_t target, uint32_t buffer, uint32_t offset, uint32_t size) {
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    void* pointer = glMapBufferRange(GL_UNIFORM_BUFFER, offset, size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
}

void RenderLib::buffer_binding_range(uint32_t buffer, uint32_t binding, uint32_t offset, uint32_t size) {
    glBindBufferRange(GL_UNIFORM_BUFFER, binding, buffer, offset, size);
}

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
}

void RenderLib::culling(uint32_t mode) {
    glCullFace(mode);
} void RenderLib::front_face(uint32_t face) {
    glFrontFace(face);
}