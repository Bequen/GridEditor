#include "SceneSprite.h"

#include "Rendering/TextureLib.h"

#include <cstring>

SceneSprite::SceneSprite() :
grid(), sprite({0}) {
    animations = nullptr;
    animationBufferSize = animationsCount = 0;
    currentAnimation = 0;
}

SceneSprite::SceneSprite(uint32_t size) :
grid(size), sprite(Sprite(TextureLib::texture_size(size, size))) {
    width = height = size;
    animations = nullptr;
    animationBufferSize = animationsCount = 0;
    currentAnimation = 0;
}

SceneSprite::SceneSprite(uint32_t width, uint32_t height) :
grid(width, height), sprite(Sprite(TextureLib::texture_size(width, height))), width(width), height(height) {
    memset(grid.buffer, 0, width * height);
    animations = nullptr;
    animationBufferSize = animationsCount = 0;
    currentAnimation = 0;
}


void SceneSprite::add_animation(char* name) {
    if(animationsCount == animationBufferSize) {
        animations = (SceneSpriteAnimation*)realloc(animations, sizeof(SceneSpriteAnimation) * (animationBufferSize + 8));
        if(animations != nullptr) {
            animationBufferSize += 8;
        }
    }

    SceneSpriteAnimation animation;
    animation.name = name;
    animation.add_frame(grid.width, grid.height);

    animations[animationsCount++] = animation;
}

void SceneSprite::change_animation(uint32_t index) {
    if(index >= animationsCount)
        index = animationsCount - 1;

    currentAnimation = index;

    //memcpy(grid.buffer, animations[currentAnimation].frames[animations[currentAnimation].currentFrame].grid.buffer, grid.width * grid.height);
    if(grid.buffer) {
        delete [] grid.buffer;
        grid.buffer = animations[currentAnimation].frames[animations[currentAnimation].currentFrame].grid.buffer;
    }

    update_texture();
}

void SceneSprite::set(uint32_t index, uint32_t value) {
    if(index > width * height)
        return;
    else
        grid.set(index, value);
} void SceneSprite::set(uint32_t x, uint32_t y, uint32_t value) {
    if( x > width ||
        y > height)
        return;
    else
        grid.set(x, y,  value);
} void SceneSprite::set(glm::vec2 position, uint32_t value) {
    if(position.x > width ||
        position.y > height)
        return;
    else
        grid.set(std::floor(position.x), std::floor(position.y), value);
}



const int32_t SceneSprite::get(uint32_t index) const {
    if(index > width * height)
        return -1;
    else
        return grid.get(index);
} const int32_t SceneSprite::get(uint32_t x, uint32_t y) const {
    if( x > width ||
        y > height)
        return -1;
    else
        return grid.get(x, y);
} const int32_t SceneSprite::get(glm::vec2 position) const {
    if(position.x > width ||
        position.y > height)
        return -1;
    else
        return grid.get(std::floor(position.x), std::floor(position.y));
}



void SceneSprite::update_texture() const {
    TextureLib::update_texture_2d(sprite.texture, grid.width, grid.height, grid.buffer);
}

const bool SceneSprite::intersects(glm::vec2 position) const {
    return position.x > 0.0f && position.x < width &&
            position.y > 0.0f && position.y < height;
}

void SceneSprite::resize(uint32_t width, uint32_t height) {
    if(this->width > width && this->height > height) {

    } else {
        if(sprite.size > width && sprite.size > height) {

        } else {
            sprite.delete_texture();
            sprite = Sprite(TextureLib::texture_size(width, height));
        }
    }

    this->width = width;
    this->height = height;

    grid.resize(width, height);
    update_texture();
}