#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#define CACHE_SIZE 32

struct _CacheCell {
    int32_t index;

    int8_t newValue;
    int8_t oldValue;
};

struct SubGrid {
    int8_t buffer[16 * 16 * 16];
    
};

/**
 * @brief  Grid containing buffer of indexes to palette
 * @note   
 * @retval None
 * @TODO a) Split grid into smaller grids for greedy meshing
 */
struct Grid {
    uint32_t gridTexture;

    int8_t* buffer;
    uint32_t width, depth, height;

    uint32_t max;
    uint32_t min;

    _CacheCell* cache[CACHE_SIZE];
    uint32_t cacheLengths[CACHE_SIZE];
    uint32_t cacheIndex;
    uint32_t cacheDepth;

    Grid();
    Grid(uint32_t size);
    Grid(uint32_t width, uint32_t depth, uint32_t height);

    int32_t get(uint32_t index);
    int32_t get(glm::vec3 pos);

    void set(uint32_t index, int8_t value);
    void set(glm::vec3 pos, int8_t value);

    bool intersects(glm::vec3 point);

    void resize(uint32_t width, uint32_t depth, uint32_t height);
};