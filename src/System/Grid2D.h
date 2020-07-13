#pragma once

#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <memory>

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

    void set(uint32_t index, T value) {
        #if DEBUG
            if(index > width * height)
                return;
        #endif
            buffer[index] = value;
    }

    void set(uint32_t x, uint32_t y, T value) {
        #if DEBUG
            if(x >= width || y >= height)
                return;
        #endif
            buffer[x + y * width] = value;
    }

    const T get(uint32_t index) const {
        if(index > width * height)
            return T();
        else
            return buffer[index];
    }

    const T get(uint32_t x, uint32_t y) const {
        if(x >= width || y >= height)
            return T();
        else
            return buffer[x + y * width];
    }

    void resize(uint32_t width, uint32_t height) {
        T* newBuffer = new T[width * height];
        memset(newBuffer, 0, width * height);

        uint32_t index = 0;
        uint32_t newIndex = 0;
        for(uint32_t z = 0; z < std::min(height, this->height); z++) {
            for(uint32_t x = 0; x < std::min(width, this->width); x++) {
                newBuffer[x + z * width] = buffer[x + z * this->width];
            } 
            //index += std::max(width, this->width) - std::min(width, this->width);
        } 
        //index += std::max(height, this->height) - std::min(height, this->height);

        delete [] buffer;

        buffer = newBuffer;
        this->width = width;
        this->height = height;
    }
};