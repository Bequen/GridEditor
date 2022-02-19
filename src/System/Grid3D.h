#pragma once

#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <iostream>

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
        memset(buffer, 0, size * size * size * sizeof(T));
    }

    Grid3D(uint32_t width, uint32_t height, uint32_t depth) :
    buffer(new T[width * height * depth]), width(width), height(height), depth(depth) {
        memset(buffer, 0, width * height * depth * sizeof(T));
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

    int32_t get(uint32_t index) {
        if(index > width * depth * height)
            return -1;
        else
            return buffer[index];
    }

    inline void
    dup_buffer() {
        T* temp = new T[width * height * depth];
        memcpy(temp, buffer, width * height * depth * sizeof(T));
        buffer = temp;
    }

    void resize(uint32_t width, uint32_t depth, uint32_t height) {
        T* newBuffer = new T[width * depth * height];
        memset(newBuffer, 0, width * depth * height);

        uint32_t index = 0;
        uint32_t newIndex = 0;
        for(uint32_t z = 0; z < std::min(height, this->height); z++) {
            for(uint32_t y = 0; y < std::min(depth, this->depth); y++) {
                for(uint32_t x = 0; x < std::min(width, this->width); x++) {
                    newBuffer[x + y * width + z * (width * depth)] = buffer[x + y * this->width + z * (this->width * this->depth)];
                }
            }
        }

        delete [] buffer;

        buffer = newBuffer;
        this->width = width;
        this->depth = depth;
        this->height = height;
    }
};
