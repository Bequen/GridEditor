#pragma once

#include "Quad.h"
#include <cstdint>
#include <glm/glm.hpp>

#define DIR_Z 0x0000
#define DIR_Y 0x0001
#define DIR_X 0x0002

namespace RenderLib {
    void init();
    void update();

    uint32_t create_voxel();
    uint32_t create_quad_top();
    uint32_t create_quad_side();
    uint32_t create_quad_forward();
    
    void bind_vertex_array(uint32_t VAO);
    void draw_triangles(uint32_t triangles);

    void culling(uint32_t mode);
    void front_face(uint32_t face);

    uint32_t create_buffer_stream(uint32_t target, uint32_t size, void* data);

    void* map_buffer_stream(uint32_t target, uint32_t buffer, uint32_t offset, uint32_t size);
    void buffer_binding_range(uint32_t buffer, uint32_t binding, uint32_t offset, uint32_t size);

    void draw_voxel(uint32_t program, float x, float y, float z);
    void draw_voxel(uint32_t program, glm::vec3 position);
    void draw_voxel(uint32_t program, glm::vec3 position, glm::vec3 scale);


    glm::vec3 get_voxel(uint32_t dir, float x, float y, float z);
    glm::vec3 get_adjacent_voxel(uint32_t dir, float x, float y, float z, float normal);

    void draw_quad(Quad quad, uint32_t dir, uint32_t negative);
    void draw_quad_z(Quad quad, uint32_t negative);
    void draw_quad_y(Quad quad, uint32_t negative);
    void draw_quad_x(Quad quad, uint32_t negative);
};