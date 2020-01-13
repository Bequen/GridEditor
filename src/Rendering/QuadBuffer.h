#pragma once

#include <cstdint>
#include "Quad.h"

struct QuadBuffer {
    Quad** quads[2];
    uint32_t* counts[2];
    uint32_t streak[2];
    uint32_t quadIndex[2];

    QuadBuffer() {

    }

    QuadBuffer(uint32_t size) :
    quads({ new Quad*[size], new Quad*[size] }), counts({ new uint32_t[size], new uint32_t[size] }), streak({ false, false }), quadIndex({ 0, 0 }) {

    }
};