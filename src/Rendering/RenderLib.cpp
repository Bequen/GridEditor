#include "RenderLib.h"

#include <glad/glad.h>
#include "ShaderLib.h"

void RenderLib::init() {
    glEnable(GL_DEPTH_TEST);
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
    glm::vec3 pos = glm::vec3(position.x, position.y, position.z);
    ShaderLib::uniform_vec3(program, "position", &pos[0]);
    ShaderLib::uniform_mat4(program, "model", &model[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 36);
}