#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace RenderLib {
    void init();
    void update();

    uint32_t create_voxel();
    void bind_vertex_array(uint32_t VAO);
    void draw_triangles(uint32_t triangles);

    uint32_t create_buffer_stream(uint32_t target, uint32_t size, void* data);

    void* map_buffer_stream(uint32_t target, uint32_t buffer, uint32_t offset, uint32_t size);
    void buffer_binding_range(uint32_t buffer, uint32_t binding, uint32_t offset, uint32_t size);

    void draw_voxel(uint32_t program, float x, float y, float z);
    void draw_voxel(uint32_t program, glm::vec3 position);
};