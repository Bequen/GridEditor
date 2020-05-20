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

    void init(uint32_t size) {
        buffer = new GridCacheCell[size];
        count = 0;
    }
};