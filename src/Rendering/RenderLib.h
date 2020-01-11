#pragma once

#include "Quad.h"
#include <cstdint>
#include <glm/glm.hpp>

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
    void draw_quad_y(Quad quad);
    void draw_quad_x(Quad quad);
};