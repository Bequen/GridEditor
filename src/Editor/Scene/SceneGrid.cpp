#include "SceneGrid.h"

#define CACHE_DEPTH 32

#include "Rendering/TextureLib.h"

#include <avg/Debug.h>

SceneGrid::SceneGrid() {
    voxelGrid = VoxelGrid(32);
    grid = Grid3D<int8_t>(32);
    cache = new GridCache[CACHE_DEPTH];
    for(uint32_t i = 0; i < CACHE_DEPTH; i++) {
        cache[i].init(512);
    }

    cacheDepth = 0;
    cacheSize = CACHE_DEPTH;
    cacheIndex = 0;
}

SceneGrid::SceneGrid(uint32_t size) {
    voxelGrid = VoxelGrid(size, size, size);
    grid = Grid3D<int8_t>(size);

    cache = new GridCache[CACHE_DEPTH];
    for(uint32_t i = 0; i < CACHE_DEPTH; i++) {
        cache[i].init(512);
    }

    cacheDepth = 0;
    cacheSize = CACHE_DEPTH;
    cacheIndex = 0;
}

SceneGrid::SceneGrid(uint32_t width, uint32_t depth, uint32_t height) {
    voxelGrid = VoxelGrid(width, height, depth);
    grid = Grid3D<int8_t>(width, height, depth);

    cache = new GridCache[CACHE_DEPTH];
    for(uint32_t i = 0; i < CACHE_DEPTH; i++) {
        cache[i].init(512);
    }

    cacheDepth = 0;
    cacheSize = CACHE_DEPTH;
    cacheIndex = 0;
}


void SceneGrid::set(uint32_t index, uint32_t value) {
    if(index > grid.width * grid.height * grid.depth)
        return;
    else {
        if(get(index) != value) {
            SUCCESS("Yaayy");
            cache[cacheDepth].buffer[cache[cacheDepth].count++] = {index, grid.buffer[index], value};
            grid.buffer[index] = value;
        } else {
            ERROR("Fuck " << value);
        }
    }
}

void SceneGrid::set(glm::vec3 position, uint32_t value) {
    uint32_t index = std::floor(position.x) + std::floor(position.y) * grid.width + std::floor(position.z) * (grid.width * grid.depth);
    if(index > grid.width * grid.height * grid.depth)
        return;
    else {
        if(get(index) != value) {
            SUCCESS("Yaayy");
            cache[cacheDepth].buffer[cache[cacheDepth].count++] = {index, grid.buffer[index], value};
            grid.buffer[index] = value;
        } else {
            ERROR("Fuck " << value);
        }
    }

}

void SceneGrid::set(uint32_t x, uint32_t y, uint32_t z, uint32_t value) {
    uint32_t index = x + y * grid.width + z * (grid.width * grid.depth);
    if(index > grid.width * grid.height * grid.depth)
        return;
    else {
        if(get(index) != value) {
            SUCCESS("Yaayy");
            cache[cacheDepth].buffer[cache[cacheDepth].count++] = {index, grid.buffer[index], value};
            grid.buffer[index] = value;
        } else {
            ERROR("Fuck " << value);
        }
    }
}


const int32_t SceneGrid::get(uint32_t index) const {
    if(index > grid.width * grid.height * grid.depth)
        return -1;
    else
        return grid.buffer[index];
}

const int32_t SceneGrid::get(glm::vec3 position) const {
    if(position.x < 0 || position.x > grid.width ||
        position.y < 0 || position.y > grid.depth ||
        position.z < 0 || position.z > grid.height)
        return -1;
    uint32_t index = std::floor(position.x) + std::floor(position.y) * grid.width + std::floor(position.z) * (grid.width * grid.depth);
    return grid.buffer[index];
}

const int32_t SceneGrid::get(uint32_t x, uint32_t y, uint32_t z) const {
    uint32_t index = x + y * grid.width + z * (grid.width * grid.depth);
    if(index > grid.width * grid.height * grid.depth)
        return -1;
    else
        return grid.buffer[index];
}


void SceneGrid::update_texture() {
    TextureLib::update_texture_3d(voxelGrid.texture, grid.width, grid.height, grid.depth, grid.buffer);
}

bool SceneGrid::intersects(glm::vec3 position) {
    return  position.x < grid.width && position.x > 0.0f &&
            position.y < grid.depth && position.y > 0.0f &&
            position.z < grid.height && position.z > 0.0f;
}


void SceneGrid::undo() {
    if(cacheDepth > cacheIndex) {
        MESSAGE("Undo " << cacheIndex << " for " << cache[cacheIndex].count);
        cacheIndex++;
        for(uint32_t i = 0; i < cache[cacheDepth - cacheIndex].count; i++) {
            set(cache[cacheDepth - cacheIndex].buffer[i].index, cache[cacheDepth - cacheIndex].buffer[i].color);
        }
    }
}

void SceneGrid::redo() {
    if(cacheIndex > 0) {
        MESSAGE("Redo " << cacheIndex << " for " << cache[cacheIndex].count);
        for(uint32_t i = 0; i < cache[cacheDepth - cacheIndex].count; i++) {
            set(cache[cacheDepth - cacheIndex].buffer[i].index, cache[cacheDepth - cacheIndex].buffer[i].newColor);
        }
        cacheIndex--;
    }
}