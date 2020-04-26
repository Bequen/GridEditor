#pragma once

#include <cstdint>

template<typename T>
struct Grid3D {
    T* buffer;
    uint32_t width, height, depth;

    Grid3D() :
    buffer(nullptr) {
        width = height = depth = 0;
    }

    Grid3D(uint32_t size) {
        buffer = new T[size * size * size];
        width = height = depth = size;
    }

    Grid3D(uint32_t width, uint32_t height, uint32_t depth) :
    buffer(new T[width * height * depth]), width(width), height(height), depth(depth) {
        
    }

    void set(uint32_t x, uint32_t y, uint32_t z, uint32_t value) {
        #if DEBUG
        if(x > width || y > depth || z > height)
            return;
        #endif
        buffer[x + y * width + z * (width * depth)] = value;
    }

    int32_t get(uint32_t x, uint32_t y, uint32_t z) {
        if(x > width || y > depth || z > height)
            return -1;
        else
            return buffer[x + y * width + z * (width * depth)];
    }
};