#pragma once

#include <cstdint>

#include "Framebuffer.h"

namespace TextureLib {
    uint32_t create_texture_1d(uint32_t width, uint32_t internalFormat, uint32_t format, void* data);
    void update_texture_1d(uint32_t texture, uint32_t width, void* data);

    uint32_t create_texture_3d(uint32_t width, uint32_t height, uint32_t depth, void* data);
    void update_texture_3d(uint32_t texture, uint32_t width, uint32_t height, uint32_t depth, void* data);

    uint32_t create_texture_2d(uint32_t target, uint32_t width, uint32_t height, uint32_t type, uint32_t format, uint32_t internalFormat, void* data);

    Framebuffer create_framebuffer(uint32_t width, uint32_t height);

    void framebuffer_attachment(uint32_t texture, uint32_t target, uint32_t type);

    void delete_texture(uint32_t texture);
};