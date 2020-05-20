#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "System/Grid3D.h"
#include "Editor/VoxelGrid.h"
#include "System/Voxels/GridCache.h"

struct SceneGrid {
    VoxelGrid voxelGrid;
    Grid3D<int8_t> grid;

    GridCache* cache;
    uint32_t cacheDepth;
    uint32_t cacheSize;
    uint32_t cacheIndex;

    SceneGrid();
    SceneGrid(uint32_t size);
    SceneGrid(uint32_t width, uint32_t depth, uint32_t height);

    void set(uint32_t index, uint32_t value);
    void set(glm::vec3 position, uint32_t value);
    void set(uint32_t x, uint32_t y, uint32_t z, uint32_t value);

    const int32_t get(uint32_t index) const;
    const int32_t get(glm::vec3 position) const;
    const int32_t get(uint32_t x, uint32_t y, uint32_t z) const;

    const int32_t width() const {
        return grid.width;
    }
    const int32_t height() const {
        return grid.height;
    }
    const int32_t depth() const {
        return grid.depth;
    }

    void update_texture();

    bool intersects(glm::vec3 position);


    void undo();
    void redo();
};