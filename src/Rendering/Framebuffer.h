#pragma once

#include <cstdint>

struct Framebuffer {
    uint32_t framebuffer;
    uint32_t width, height;
    uint32_t texture, depth;
};