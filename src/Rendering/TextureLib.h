#pragma once

#include <cstdint>

namespace TextureLib {
    uint32_t create_texture_3d(uint32_t width, uint32_t height, uint32_t depth, void* data);
    void update_texture_3d(uint32_t texture, uint32_t width, uint32_t height, uint32_t depth, void* data);
};