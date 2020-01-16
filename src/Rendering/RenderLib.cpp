#include "RenderLib.h"

#include <glad/glad.h>
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
    
    glEnable(GL_MULTISAMPLE);
    
    RenderLib::culling(GL_BACK);
}

void RenderLib::update() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2, 0.0, 0.5, 1.0);
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
        0.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        0.0f,   0.0f,   1.0f,
        1.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   1.0f,

        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   0.0f,   1.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   1.0f,


        // Opposite direction
        0.0f,   1.0f,   0.0f,
        0.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        1.0f,   1.0f,   0.0f,

        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   0.0f,
        0.0f,   0.0f,   1.0f,
        1.0f,   0.0f,   0.0f,
        1.0f,   0.0f,   1.0f,

        0.0f,   0.0f,   1.0f,
        0.0f,   0.0f,   0.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   0.0f,   1.0f,
        0.0f,   1.0f,   0.0f,
        0.0f,   1.0f,   1.0f,
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
#pragma endregion

void RenderLib::bind_vertex_array(uint32_t VAO) {
    glBindVertexArray(VAO);
}

void RenderLib::draw_triangles(uint32_t triangles) {
    glDrawArrays(GL_TRIANGLES, 0, triangles);
}

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


glm::vec3 RenderLib::get_adjacent_voxel(uint32_t dir, float x, float y, float z, float normal) {
    glm::vec3 pos[3] = { {x, y, z + normal}, {x, z + normal, y}, {z + normal, x, y} };
    return pos[dir];
}

glm::vec3 RenderLib::get_voxel(uint32_t dir, float x, float y, float z) {
    glm::vec3 pos[3] = { {x, y, z}, {x, z, y}, {z, x, y} };
    return pos[dir];
}

void RenderLib::draw_quad(Quad quad, uint32_t dir, uint32_t negative) {
    switch(dir) {
        case DIR_X:
            draw_quad_x(quad, negative);
            break;
        case DIR_Y:
            draw_quad_y(quad, negative);
            break;
        case DIR_Z:
            draw_quad_z(quad, negative);
            break;
    }
}

void RenderLib::draw_quad_z(Quad quad, uint32_t negative) {
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    quad.h = quad.z + 1.0f;
    ShaderLib::uniform_vec3(program, "position", &quad.x);
    ShaderLib::uniform_vec3(program, "scale", &quad.w);

    glDrawArrays(GL_TRIANGLES, 0 + negative * 18, 6);
} void RenderLib::draw_quad_y(Quad quad, uint32_t negative) {
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    float x = quad.x;
    float y = quad.y;
    float z = quad.z;

    quad.x = x;
    quad.y = z;
    quad.z = y;
    quad.h = quad.d;
    quad.d = quad.y + 1.0f;
    ShaderLib::uniform_vec3(program, "position", &quad.x);

    ShaderLib::uniform_vec3(program, "scale", &quad.w);

    glDrawArrays(GL_TRIANGLES, 6 + negative * 18, 6);
} void RenderLib::draw_quad_x(Quad quad, uint32_t negative) {
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    quad.h = quad.d;
    quad.d = quad.w;
    quad.w = quad.x + 1.0f;
    float x = quad.x;
    float y = quad.y;
    float z = quad.z;

    quad.x = z;
    quad.y = x;
    quad.z = y;

    ShaderLib::uniform_vec3(program, "position", &quad.x);
    ShaderLib::uniform_vec3(program, "scale", &quad.w);

    glDrawArrays(GL_TRIANGLES, 12 + negative * 18, 6);
}

void RenderLib::culling(uint32_t mode) {
    glCullFace(mode);
} void RenderLib::front_face(uint32_t face) {
    glFrontFace(face);
}