#pragma once

#include <cstdint>
#include <cstring>

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
        count = 0;
        size = size;
    }

    void resize(uint32_t newSize) {
        GridCacheCell* temp = new GridCacheCell[newSize];
        memcpy(temp, buffer, sizeof(GridCacheCell) * size);

        delete [] buffer;
        buffer = temp;

        size = newSize;
    }
};