#include "TextureLib.h"

#include <glad/glad.h>

uint32_t TextureLib::create_texture_1d(uint32_t width, uint32_t internalFormat, uint32_t format, void* data) {
    uint32_t result;
    glGenTextures(1, &result);

    glBindTexture(GL_TEXTURE_1D, result);
    glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, width, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    return result;
}

void TextureLib::update_texture_1d(uint32_t texture, uint32_t width, void* data) {
    glBindTexture(GL_TEXTURE_1D, texture);
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, width, GL_RGB, GL_UNSIGNED_BYTE, data);
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
}

void TextureLib::update_texture_3d(uint32_t texture, uint32_t width, uint32_t height, uint32_t depth, void* data) {
    glBindTexture(GL_TEXTURE_3D, texture);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, GL_RED, GL_UNSIGNED_BYTE, data);
}