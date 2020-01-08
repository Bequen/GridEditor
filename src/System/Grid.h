#pragma once

#include <cstring>
#include <cmath>
#include <glm/glm.hpp>
#include <avg/Debug.h>

template<typename T>
struct Grid {
    T* grid;
    uint32_t size;

    glm::vec3 position;

    Grid() :
    size(0), grid(nullptr) {

    }

    Grid(uint32_t size) :
    size(size), grid(new T[size * size * size]) {
        memset(grid, 0, size * size * size * sizeof(T));
    }

    void init(uint32_t size) {
        this->size = size;
        this->grid = (T*)malloc(sizeof(T) * (size * size * size));

        memset(this->grid, 0, this->size * this->size * this->size * sizeof(T));
    } void init(uint32_t size, T* buffer) {
        this->size = size;
        grid = new T(size * size * size);
        memcpy(grid, buffer, sizeof(T) * (size * size * size));

        //memset(grid, 0, size * size * size * sizeof(T));
    }

    void set(uint32_t index, T data) {
        grid[index] = data;
    } void set(uint32_t x, uint32_t y, uint32_t z, T data) {
        grid[x + y * size + z * (size * size)] = data;
    } bool set(glm::vec3 position, T data) {
        if(position.x < 0.0f || position.x > size ||
            position.y < 0.0f || position.y > size ||
            position.z < 0.0f || position.z > size)
            return false;
        grid[(uint32_t)std::floor(position.x) + (uint32_t)std::floor(position.y) * size + (uint32_t)std::floor(position.z) * (size * size)] = data;
        return true;
    }

    T get(uint32_t index) {
        return grid[index];
    } T get(uint32_t x, uint32_t y, uint32_t z) {
        return grid[x + y * size + z * (size * size)];
    } int32_t get(glm::vec3 position) {
        if(position.x >= 0.0f && position.x < size &&
            position.y >= 0.0f && position.y < size &&
            position.z >= 0.0f && position.z < size)
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