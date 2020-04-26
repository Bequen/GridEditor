#pragma once

#include <cstdint>

template<typename T>
struct Grid2D {
    T* buffer;
    uint32_t width, height;

    Grid2D() :
    buffer(nullptr), width(0), height(0) {
        
    }

    Grid2D(uint32_t size) :
    buffer(new T[size * size]), width(size), height(size) {
        
    }

    Grid2D(uint32_t width, uint32_t height) :
    buffer(new T[width * height]), width(width), height(height) {

    }

    void set(uint32_t x, uint32_t y, uint32_t value) {
        #if DEBUG
        if(x >= width || y >= height)
            return;
        #endif
        buffer[x + y * width] = value;
    }

    int32_t get(uint32_t x, uint32_t y) {
        if(x >= width || y >= height)
            return -1;
        else
            return buffer[x + y * width];
    }
};