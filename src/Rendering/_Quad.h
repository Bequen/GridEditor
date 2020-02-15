#pragma once

#include <cstdint>

struct _Quad {
    uint32_t x, y, z;
    uint32_t width, depth, height;

    _Quad() :
    x(0), y(0), z(0), width(0), depth(0), height(0) {
        
    }

    _Quad(uint32_t x, uint32_t y, uint32_t z, uint32_t width, uint32_t depth, uint32_t height) :
    x(x), y(y), z(z), width(width), depth(depth), height(height) {

    }
};