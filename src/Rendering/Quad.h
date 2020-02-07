#pragma once

#include <cstdint>

struct Quad {
    float x, y, z, w, d, h;
    uint32_t brush;

    Quad() {}

    Quad(float x, float y, float z, float d, uint32_t brush) :
    x(x), y(y), z(z), w(0.0f), d(d), h(0.0f), brush(brush) {

    }

    Quad(float x, float y, float z, float w, float d, float h) :
    x(x), y(y), z(z), w(w), d(d), h(h) {

    }
};