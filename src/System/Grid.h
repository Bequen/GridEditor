#pragma once

#include <cstring>
#include <cmath>
#include <glm/glm.hpp>

template<typename T>
struct Grid {
    T* grid;
    uint32_t size;

    glm::vec3 position;

    Grid(uint32_t size) :
    size(size), grid(new T[size * size * size]) {
        memset(grid, 0, size * size * size * sizeof(T));
    }

    void set(uint32_t index, T data) {
        grid[index] = data;
    } void set(uint32_t x, uint32_t y, uint32_t z, T data) {
        grid[x + y * size + z * (size * size)] = data;
    } void set(glm::vec3 position, T data) {
        if(position.x < 0 || position.x > size ||
            position.y < 0 || position.y > size ||
            position.z < 0 || position.z > size)
            return;
        grid[(uint32_t)std::floor(position.x) + (uint32_t)std::floor(position.y) * size + (uint32_t)std::floor(position.z) * (size * size)] = data;
    }

    T get(uint32_t index) {
        return grid[index];
    } T get(uint32_t x, uint32_t y, uint32_t z) {
        return grid[x + y * size + z * (size * size)];
    } T get(glm::vec3 position) {
        if(position.x > 0 && position.x < size &&
            position.y > 0 && position.y < size &&
            position.z > 0 && position.z < size)
            return grid[(uint32_t)std::floor(position.x) + (uint32_t)std::floor(position.y) * size + (uint32_t)std::floor(position.z) * (size * size)];
        else
            return -1;
    }

    bool point_intersection(glm::vec3 point) {
        if(point.x > position.x && point.y > position.y && point.z > position.z &&
            point.x < position.x + size && point.y < position.y + size && point.z < position.z + size) {
            return true;
        } else {
            return false;
        }
    }
};