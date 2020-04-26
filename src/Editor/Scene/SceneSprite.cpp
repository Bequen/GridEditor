#include "SceneSprite.h"

SceneSprite::SceneSprite() :
grid(), sprite({0}) {
    
}

SceneSprite::SceneSprite(uint32_t size) :
grid(size), sprite(Sprite()) {

}

SceneSprite::SceneSprite(uint32_t width, uint32_t height) :
grid(width, height), sprite(Sprite()) {
    
}