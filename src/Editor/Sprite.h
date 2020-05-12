#pragma once

#include <cstdint>

#include "System/Transform.h"

struct Sprite {
    uint32_t texture;
    uint32_t width, height;

    Sprite();
    Sprite(uint32_t size);
    Sprite(uint32_t width, uint32_t height);
};