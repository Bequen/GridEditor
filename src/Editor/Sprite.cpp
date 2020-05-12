#include "Sprite.h"

#include <glad/glad.h>

#include "Rendering/TextureLib.h"

Sprite::Sprite() :
texture(0) {
    width = height = 0;
} Sprite::Sprite(uint32_t size) {
    width = height = size;
    texture = TextureLib::create_texture_2d(GL_TEXTURE_2D, size, size, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr);
} Sprite::Sprite(uint32_t width, uint32_t height) :
width(width), height(height) {
    texture = TextureLib::create_texture_2d(GL_TEXTURE_2D, width, height, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr);
}