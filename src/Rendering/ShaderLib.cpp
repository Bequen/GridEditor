#include "ShaderLib.h"

#include "System/ContentPipeline.h"

#include <glad/glad.h>
#include <csignal>
#include <iostream>
#include <avg/Debug.h>

uint32_t ShaderLib::program_create(char* name) {
    MESSAGE("Creating program " << name);
    uint32_t program = glCreateProgram();

    // Creates path to the vertex shader
    string256 path = ContentPipeline::asset_path("shaders/", name, ".vert");
    uint32_t vertex = shader_create(GL_VERTEX_SHADER, path.str);

    assert_msg(vertex != FAILED_SHADER_COMPILATION, "Failed compiling the vertex shader");

    path.length = 0;

    // Creates path to the fragment shader
    path = ContentPipeline::asset_path("shaders/", name, ".frag");
    uint32_t fragment = shader_create(GL_FRAGMENT_SHADER, path.str);

    assert_msg(fragment != FAILED_SHADER_COMPILATION, "Failed compiling the fragment shader");

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);

    #if DEBUG
        int32_t success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if(!success) {
            int32_t maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            char* info = new char[maxLength];
            glGetProgramInfoLog(program, maxLength, &maxLength, info);

            glDeleteProgram(program);

            glDeleteShader(vertex);
            glDeleteShader(fragment);

            delete [] info;
            assert_msg(false, "Program linking error");
        }
    #endif

    glDetachShader(program, vertex);
    glDetachShader(program, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    SUCCESS("Creating program " << name << " was successfull");

    return program;
}

int32_t ShaderLib::shader_create(uint32_t type, char* path) {
    MESSAGE("Creating shader")

    uint32_t shader = glCreateShader(type);

    uint32_t length;
    char* source = ContentPipeline::load_text(path, &length);
    glShaderSource(shader, 1, &source, 0);
    
    glCompileShader(shader);

    #if DEBUG
        int32_t success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success) {
            int32_t maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            char* info = new char[maxLength];
            glGetShaderInfoLog(shader, maxLength, &maxLength, info);

            std::cout << "Shader has compile errors!" << std::endl;
            std::cout << info << std::endl;
            
            delete [] info;
            delete [] source;

            glDeleteShader(shader);
            return FAILED_SHADER_COMPILATION;
        }
    #endif

    delete [] source;
    SUCCESS("Shader created successfully");

    return shader;
}

void ShaderLib::program_use(uint32_t program) {
    glUseProgram(program);
}


void ShaderLib::uniform_mat4(uint32_t program, char* name, float* data) {
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, data);
}

void ShaderLib::uniform_vec2(uint32_t program, char* name, float* data) {
    glUseProgram(program);
    glUniform2fv(glGetUniformLocation(program, name), 1, data);
}

void ShaderLib::uniform_vec3(uint32_t program, char* name, float* data) {
    glUseProgram(program);
    glUniform3fv(glGetUniformLocation(program, name), 1, data);
} void ShaderLib::uniform_vec4(uint32_t program, char* name, float* data) {
    glUseProgram(program);
    glUniform4fv(glGetUniformLocation(program, name), 1, data);
}

void ShaderLib::uniform_int32(uint32_t program, char* name, int32_t data) {
    glUseProgram(program);
    glUniform1i(glGetUniformLocation(program, name), data);
}