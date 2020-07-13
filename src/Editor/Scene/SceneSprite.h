#pragma once

#include "Editor/Sprite.h"
#include "System/Grid2D.h"
#include "Editor/Color.h"
#include "SceneSpriteAnimation.h"

// TODO
// Grid should be just and only grid, without texture
// VoxelGrid should have texture and transform
// Sprite should have texture and transform
// SceneSprite should have Grid and Sprite
// SceneVoxelGrid should have Grid and VoxelGrid
// SceneVoxelGrid should manage the setting of voxels, so it can be further extended into subgrids etc.


struct SceneSprite {
    Sprite sprite;
    Grid2D<char> grid;

    SceneSpriteAnimation* animations;
    uint32_t animationBufferSize;
    uint32_t animationsCount;
    uint32_t currentAnimation;

    uint32_t width, height;

    SceneSprite();
    SceneSprite(uint32_t size);
    SceneSprite(uint32_t width, uint32_t height);

    void add_animation(char* name);

    void set(uint32_t index, uint32_t value);
    void set(glm::vec2 position, uint32_t value);
    void set(uint32_t x, uint32_t y, uint32_t value);

    const int32_t get(uint32_t index) const;
    const int32_t get(glm::vec2 position) const;
    const int32_t get(uint32_t x, uint32_t y) const;

    void update_texture() const;

    const bool intersects(glm::vec2 position) const;

    void resize(uint32_t width, uint32_t height);
};