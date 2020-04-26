#pragma once

#include "Editor/Sprite.h"
#include "System/Grid2D.h"
#include "Editor/Color.h"

// TODO
// Grid should be just and only grid, without texture
// VoxelGrid should have texture and transform
// Sprite should have texture and transform
// SceneSprite should have Grid and Sprite
// SceneVoxelGrid should have Grid and VoxelGrid
// SceneVoxelGrid should manage the setting of voxels, so it can be further extended into subgrids etc.


struct SceneSprite {
    Sprite sprite;
    Grid2D<RGB> grid;

    SceneSprite();
    SceneSprite(uint32_t size);
    SceneSprite(uint32_t width, uint32_t height);

    void set(uint32_t x, uint32_t y, RGB color);
};