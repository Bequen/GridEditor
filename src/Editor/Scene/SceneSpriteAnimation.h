#pragma once

#include "SceneSprite.h"

struct SceneSpriteAnimationFrame {
    Grid2D<char> grid;

    float frameLength;

    SceneSpriteAnimationFrame(uint32_t width, uint32_t height) {
        grid = Grid2D<char>(width, height);
    }
};

struct SceneSpriteAnimation {
    char* name;

    SceneSpriteAnimationFrame* frames;
    uint32_t frameBufferSize;
    uint32_t frameCount;
    uint32_t currentFrame;

    SceneSpriteAnimation() :
    frames(nullptr), frameBufferSize(0), frameCount(0) {
        currentFrame = 0;
    }

    void add_frame(uint32_t width, uint32_t height) {
        if(frameBufferSize == frameCount) {
            frames = (SceneSpriteAnimationFrame*)realloc(frames, sizeof(SceneSpriteAnimationFrame) * (frameBufferSize + 32));
            if(frames) {
                frameBufferSize += 32;
            }
        }

        frames[frameCount++] = SceneSpriteAnimationFrame(width, height);
    }
};