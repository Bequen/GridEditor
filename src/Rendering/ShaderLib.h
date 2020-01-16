#pragma once

#include <cstdint>

#define FAILED_LINKING_PROGRAM -1
#define FAILED_SHADER_COMPILATION -2

#define FAILED_SHADER_VERTEX -3
#define FAILED_SHADER_FRAGMENT -4


/**
 * @brief  Small namespace containing functions related to programs and shaders
 * @note   
 * @retval None
 */
namespace ShaderLib {
    /**
     * @brief  Creates the program from name of the shaders
     * @note   Automatically search in assets directory
     * @param  name: Name of the shader, without glsl extension
     * @retval returns program id
     */
    uint32_t program_create(char* name);
    /**
     * @brief  Bounds the program
     * @note   Bounds the program for use for rendering
     * @param  program: Program to be bounded
     * @retval None
     */
    void program_use(uint32_t program);


    /**
     * @brief  Creates the shader of type from file on the path
     * @note   Used internally in ShaderLib program_create function, shouldn't be used any other way
     * @param  type: Type of the program to be created
     * @param  path: Path to the shader
     * @retval Returns the shader id
     */
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