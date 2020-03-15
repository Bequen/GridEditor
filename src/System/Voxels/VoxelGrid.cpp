#include "VoxelGrid.h"

void VoxelGrid::redo() {
    if(cacheIndex <= 0) return;

    cacheIndex--;
    for(uint32_t i = 0; i < cacheDepths[cacheIndex]; i++) {
        grid.set(_cache[cacheIndex][i].index, _cache[cacheIndex][i].value);
    }
}

void VoxelGrid::undo() {
    if(cacheIndex == cacheDepth) return;

    cacheIndex++;
    for(uint32_t i = 0; i < cacheDepths[_cacheIndex]; i++) {
        grid.set(_cache[_cacheIndex][i].index, _cache[cacheIndex][i].value);
    }
}