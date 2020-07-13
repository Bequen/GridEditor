#pragma once

#include <cstdint>

#include "System/Transform.h"

struct Sprite {
    uint32_t texture;
    uint32_t size;

    Sprite();
    Sprite(uint32_t size);

    void delete_texture();
};