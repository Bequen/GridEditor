#include "TextureLib.h"

#include <glad/glad.h>

uint32_t TextureLib::create_texture_1d(uint32_t width, uint32_t internalFormat, uint32_t format, void* data) {
    uint32_t result;
    glGenTextures(1, &result);

    glBindTexture(GL_TEXTURE_1D, result);
    glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, width, 0, format, GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    return result;
}

void TextureLib::update_texture_1d(uint32_t texture, uint32_t width, void* data) {
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, width, GL_RGB, GL_FLOAT, data);
}



uint32_t TextureLib::create_texture_3d(uint32_t width, uint32_t height, uint32_t depth, void* data) {
    uint32_t result;
    glGenTextures(1, &result);

    glBindTexture(GL_TEXTURE_3D, result);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, width, height, depth, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    return result;
} uint32_t TextureLib::create_texture_3d(uint32_t width, uint32_t height, uint32_t depth, uint32_t internalFormat, uint32_t format, uint32_t type, void* data) {
    uint32_t result;
    glGenTextures(1, &result);

    glBindTexture(GL_TEXTURE_3D, result);
    glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, format, type, data);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    return result;
}

void TextureLib::update_texture_3d(uint32_t texture, uint32_t width, uint32_t height, uint32_t depth, void* data) {
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, GL_RED, GL_UNSIGNED_BYTE, data);
}

uint32_t TextureLib::create_texture_2d(uint32_t target, uint32_t width, uint32_t height, uint32_t internalFormat, uint32_t format, uint32_t type, void* data) {
    uint32_t result;
    glGenTextures(1, &result);
    glBindTexture(target, result);

    glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data);

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
/*     glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT); */

    return result;
} uint32_t TextureLib::create_texture_2d(uint32_t width, uint32_t height, void* data) {
    uint32_t result;
    glGenTextures(1, &result);
    glBindTexture(GL_TEXTURE_2D, result);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return result;
} void TextureLib::update_texture_2d(uint32_t texture, uint32_t width, uint32_t height, void* data) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, data);
} void TextureLib::update_sub_texture_2d(uint32_t texture, uint32_t x, uint32_t y, uint32_t width, uint32_t height, void* data) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RED, GL_UNSIGNED_BYTE, data);
}

Framebuffer TextureLib::create_framebuffer(uint32_t width, uint32_t height) {
    Framebuffer result;
    result.width = width;
    result.height = height;

    glGenFramebuffers(1, &result.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, result.framebuffer);

    return result;
}

void TextureLib::framebuffer_attachment(uint32_t texture, uint32_t target, uint32_t type) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, type, target, texture, 0);
}


void TextureLib::delete_texture(uint32_t texture) {
    glDeleteTextures(1, &texture);
}