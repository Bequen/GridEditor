#include "Sprite.h"

#include <glad/glad.h>

#include "Rendering/TextureLib.h"

Sprite::Sprite() :
texture(0), size(0) {
    
} Sprite::Sprite(uint32_t size) {
    size = size;
    texture = TextureLib::create_texture_2d(GL_TEXTURE_2D, size, size, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr);
} 

void Sprite::delete_texture() {
    TextureLib::delete_texture(texture);
}