#pragma once

#include <cstdint>

struct GridCacheCell {
    uint32_t index;
    uint32_t color;
    uint32_t newColor;
};

struct GridCache {
    GridCacheCell* buffer;
    uint32_t count;
    uint32_t size;

    void init(uint32_t size) {
        buffer = new GridCacheCell[size];
        this->size = size;
        count = 0;
    }
};