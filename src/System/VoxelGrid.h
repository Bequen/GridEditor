#pragma once

#include "Grid.h"
#include "Rendering/TextureLib.h"
#include "Rendering/QuadMesh.h"

#define CACHE_SIZE 128

struct VoxelGrid {
    Grid<int8_t>* cache;
    uint32_t cacheIndex;
    uint32_t usedCache;
    uint32_t undoCount;

    uint32_t gridTexture;

    QuadMesh quadMesh;

    VoxelGrid() :
    cache(new Grid<int8_t>[CACHE_SIZE]) {
        cacheIndex = 0;
        usedCache = 0;
        undoCount = 0;
        for(uint32_t i = 0; i < CACHE_SIZE; i++) {
            cache[i].init(32);
        }
    }

    void init() {
        gridTexture = TextureLib::create_texture_3d(32, 32, 32, cache[0].grid);
    }
};