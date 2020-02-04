#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "Quad.h"
#include "QuadMesh.h"

#define DIR_Z 0x0000
#define DIR_Y 0x0001
#define DIR_X 0x0002

#define UNIFORM_BUFFER      0x8A11
#define ARRAY_BUFFER        0x8892

/**
 * @brief  Simple namespace with usefull rendering and gpu-related functions
 * @note   
 * @retval None
 */
namespace RenderLib {
    /**
     * @brief  Initializes basic parts of OpenGL and 
     * @note   
     * @retval None
     */
    void init();
    void update();

    /**
     * @brief  Sets the polygon mode
     * @note   The options are either GL_LINE and GL_FILL
     * @param  mode: Polygon mode, either GL_LINE and GL_FILL
     * @retval None
     */
    void polygon_mode(uint32_t mode);
    /**
     * @brief  Sets the culling mode for rendering
     * @note   Default value is GL_BACK
     * @param  mode: Mode in which polygons will be cut, possible modes are GL_BACK, GL_FRONT, GL_FRONT_AND_BACK
     * @retval None
     */
    void culling(uint32_t mode);
    /**
     * @brief  Sets which face will be considered front facing
     * @note   Do not change this value unless you know what you are doing
     * @param  face: Face that will be considered front face, either GL_CCW and GL_CW
     * @retval None
     */
    void front_face(uint32_t face);


    /**
     * @brief  Creates data required to render voxel
     * @note   Set ups buffer, fills in data and manage attribute pointers
     * @retval returns id to the buffer
     */
    uint32_t create_voxel();
    /**
     * @brief  Creates data required to render quad in all 3 dimensions
     * @note   Set ups buffer, fills in data and manage attribute pointers.
     * @retval returns id to the buffer
     */
    uint32_t create_quad();
    uint32_t create_render_quad();
    


    /**
     * @brief  Bounds the vertex array
     * @note   
     * @param  VAO: Vertex array to be bounded
     * @retval None
     */
    void bind_vertex_array(uint32_t VAO);



    /**
     * @brief  Maps the buffer in the persistent mode
     * @note   Maps the buffer so you can change the data without having to manage sync
     * @param  buffer: Buffer to be mapped
     * @param  target: Buffer target
     * @param  offset: Offset from the start of the buffer
     * @param  size: Size of the mapped area
     * @retval Pointer where you can copy the new data
     */
    void* map_buffer_range(uint32_t buffer, uint32_t target, uint32_t offset, uint32_t size);

    void* map_buffer_stream_range(uint32_t buffer, uint32_t target, uint32_t offset, uint32_t size);
    /**
     * @brief  Unmaps the buffer and flushing all the data into buffer
     * @note   
     * @param  target: Target in which the buffer is currently bounded
     * @retval None
     */
    void unmap_buffer(uint32_t target);

    uint32_t create_buffer_static(uint32_t target, uint32_t size, void* data);
    uint32_t create_buffer_stream(uint32_t target, uint32_t size, void* data);
    uint32_t create_buffer_dynamic(uint32_t target, uint32_t size, void* data);



    /**
     * @brief  Sets the buffer binding
     * @note   Buffer binding is index used to map uniform buffers in shaders
     * @param  buffer: Buffer that has the data
     * @param  binding: On which binding index should it bind
     * @param  offset: Offset from the start of the buffer, use 0 in case of entire buffer
     * @param  size: Size of the mapped area
     * @retval None
     */
    void buffer_binding_range(uint32_t buffer, uint32_t binding, uint32_t offset, uint32_t size);


    glm::vec3 get_voxel(uint32_t dir, float x, float y, float z);
    glm::vec3 get_adjacent_voxel(uint32_t dir, float x, float y, float z, float normal);


    #pragma region DRAWING
    void draw_voxel(uint32_t program, float x, float y, float z);
    void draw_voxel(uint32_t program, glm::vec3 position);
    void draw_voxel(uint32_t program, glm::vec3 position, glm::vec3 scale);

    void draw_triangles(uint32_t triangles);
    void draw_triangle_strip(uint32_t triangles);

    // TODO Move into one function
    void draw_quad_mesh(QuadMesh quadMesh);

    void draw_quad(Quad quad, uint32_t dir, uint32_t negative);
    void draw_quad_z(Quad quad, uint32_t negative);
    void draw_quad_y(Quad quad, uint32_t negative);
    void draw_quad_x(Quad quad, uint32_t negative);
    #pragma endregion
};