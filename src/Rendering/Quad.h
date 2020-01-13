#pragma once

#include <cstdint>

struct Quad {
    float x, y, z, w, d, h;
    uint32_t brush;

    Quad() {}

    Quad(float x, float y, float z, float d, uint32_t brush) :
    x(x), y(y), z(z), w(0.0f), d(d), h(0.0f), brush(brush) {

    }
};