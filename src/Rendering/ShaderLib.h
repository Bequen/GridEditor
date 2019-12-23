#pragma once

#include <cstdint>

#define FAILED_LINKING_PROGRAM -1
#define FAILED_SHADER_COMPILATION -2

#define FAILED_SHADER_VERTEX -3
#define FAILED_SHADER_FRAGMENT -4

namespace ShaderLib {
    uint32_t program_create(char* name);
    void program_use(uint32_t program);

    int32_t shader_create(uint32_t type, char* path);

    // UNIFORMS
    // ========
    #pragma region UNIFORMS
    void uniform_float(uint32_t program, char* name, float data);
    void uniform_int32(uint32_t program, char* name, int32_t data);
    void uniform_uint32(uint32_t program, char* name, uint32_t data);

    void uniform_vec2(uint32_t program, char* name, float* data);
    void uniform_vec3(uint32_t program, char* name, float* data);
    void uniform_vec4(uint32_t program, char* name, float* data);

    void uniform_vec2(uint32_t program, char* name, float x, float y);
    void uniform_vec3(uint32_t program, char* name, float x, float y, float z);
    void uniform_vec4(uint32_t program, char* name, float x, float y, float z, float w);

    void uniform_mat4(uint32_t program, char* name, float* data);
    #pragma endregion
};