#include "SceneSprite.h"

#include "Rendering/TextureLib.h"

#include <cstring>

SceneSprite::SceneSprite() :
grid(), sprite({0}) {
    
}

SceneSprite::SceneSprite(uint32_t size) :
grid(size), sprite(Sprite(size)) {

}

SceneSprite::SceneSprite(uint32_t width, uint32_t height) :
grid(width, height), sprite(Sprite(width, height)) {
    memset(grid.buffer, 0, width * height);
}

void SceneSprite::set(uint32_t index, uint32_t value) {
    if(index > width() * height())
        return;
    else
        grid.set(index, value);
} void SceneSprite::set(uint32_t x, uint32_t y, uint32_t value) {
    if( x > width() ||
        y > height())
        return;
    else
        grid.set(x, y,  value);
} void SceneSprite::set(glm::vec2 position, uint32_t value) {
    if(position.x > width() ||
        position.y > height())
        return;
    else
        grid.set(std::floor(position.x), std::floor(position.y), value);
}



const int32_t SceneSprite::get(uint32_t index) const {
    if(index > width() * height())
        return -1;
    else
        return grid.get(index);
} const int32_t SceneSprite::get(uint32_t x, uint32_t y) const {
    if( x > width() ||
        y > height())
        return -1;
    else
        return grid.get(x, y);
} const int32_t SceneSprite::get(glm::vec2 position) const {
    if(position.x > width() ||
        position.y > height())
        return -1;
    else
        return grid.get(std::floor(position.x), std::floor(position.y));
}



void SceneSprite::update_texture() const {
    TextureLib::update_texture_2d(sprite.texture, grid.width, grid.height, grid.buffer);
}

const bool SceneSprite::intersects(glm::vec2 position) const {
    return position.x > 0.0f && position.x < width() &&
            position.y > 0.0f && position.y < height();
}