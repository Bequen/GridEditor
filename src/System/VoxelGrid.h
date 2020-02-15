#pragma once

#include "Grid.h"
#include "Rendering/TextureLib.h"
#include "Rendering/QuadMesh.h"
#include "Rendering/_QuadMesh.h"

#define CACHE_SIZE 128

struct VoxelGrid {
    Grid<int8_t>* cache;
    uint32_t cacheIndex;
    uint32_t usedCache;
    uint32_t undoCount;

    uint32_t gridTexture;

    QuadMesh quadMesh;
    uint32_t size;

    _QuadMesh _quadMesh;

    void init(uint32_t size) {
        this->size = size;
        quadMesh.init(size);
        usedCache = 0;
        undoCount = 0;
        cacheIndex = 0;

        cache = new Grid<int8_t>[CACHE_SIZE];
        for(uint32_t i = 0; i < CACHE_SIZE; i++) {
            cache[i].init(size);
        }

        gridTexture = TextureLib::create_texture_3d(size, size, size, cache[0].grid);
    }
};