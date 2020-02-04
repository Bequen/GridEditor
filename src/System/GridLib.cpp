#include "GridLib.h"

#include "Rendering/RenderLib.h"

QuadMesh GridLib::greedy_meshing(Grid<int8_t> grid) {
    MESSAGE("Greedy meshing");
    QuadMesh result = QuadMesh(32, 32, 32);
    for(uint32_t z = 0; z < grid.size; z++) {
        for(uint32_t y = 0; y < grid.size; y++) {
            for(uint32_t p = 0; p < 3; p++) {
                for(uint32_t i = 0; i < 2; i++) {
                    result.buffers[p][z].streak[i] = false;
                    result.buffers[p][z].quadIndex[i] = 0;

                    result.buffers[p][z].quads[i][y] = new Quad[grid.size];
                    result.buffers[p][z].counts[i][y] = 0;
                }
            }

            float dirs[2] = {1.0f, -1.0f};

            for(uint32_t x = 0; x < grid.size; x++) {
                for(uint32_t p = 0; p < 3; p++) {
                    for(uint32_t i = 0; i < 2; i++) {
                        int32_t voxel = grid.get(RenderLib::get_voxel(p, x, y, z));
                        int32_t adjacentVoxel = grid.get(RenderLib::get_adjacent_voxel(p, x, y, z, dirs[i]));
                        if(!result.buffers[p][z].streak[i]) {
                            if(voxel > 0 && adjacentVoxel <= 0) {
                                result.buffers[p][z].streak[i] = true;
                                result.buffers[p][z].quads[i][y][result.buffers[p][z].quadIndex[i]] = Quad(x, y, z, y + 1, voxel);
                            }
                        } else {
                            if(voxel != result.buffers[p][z].quads[i][y][result.buffers[p][z].quadIndex[i]].brush || adjacentVoxel > 0) {
                                result.buffers[p][z].streak[i] = false;

                                result.buffers[p][z].quads[i][y][result.buffers[p][z].quadIndex[i]].w = x;
                                result.buffers[p][z].counts[i][y]++;
                                result.buffers[p][z].quadIndex[i]++;

                                if(voxel > 0 && adjacentVoxel <= 0) { 
                                    result.buffers[p][z].streak[i] = true;
                                    result.buffers[p][z].quads[i][y][result.buffers[p][z].quadIndex[i]] = Quad(x, y, z, y + 1, voxel);
                                }
                            }
                        }
                    }
                }
            }

            for(uint32_t p = 0; p < 3; p++) {
                for(uint32_t i = 0; i < 2; i++) {
                    if(result.buffers[p][z].streak[i]) {
                        result.buffers[p][z].streak[i] = false;

                        result.buffers[p][z].quads[i][y][result.buffers[p][z].quadIndex[i]].w = grid.size;
                        result.buffers[p][z].counts[i][y]++;
                        result.buffers[p][z].quadIndex[i]++;
                    }
                }
            }
        }

        for(uint32_t p = 0; p < 3; p++) {
            for(uint32_t i = 0; i < 2; i++) {
                greedy_meshing_merging(result.buffers[p][z].quads[i], result.buffers[p][z].counts[i], 32);
            }
        }
    }

    return result;
}

void GridLib::greedy_meshing_merging(Quad**& quads, uint32_t*& counts, uint32_t size) {
    for(uint32_t y = 1; y < size; y++) {
        uint32_t x = 0;
        uint32_t merged = 0;

        for(; x < counts[y]; x++) {
            // Runs through the previous line
            for(uint32_t i = merged; i < counts[y - 1]; i++) {
                // If the quads can be merged
                if(quads[y - 1][i].x >= quads[y][x].x &&
                    quads[y - 1][i].w <= quads[y][x].w &&
                    quads[y - 1][i].brush == quads[y][x].brush) {

                    Quad previous = quads[y - 1][i];
                    Quad current = quads[y][x];
                    uint32_t index = 0;
                    uint32_t offset = 0;

                    uint32_t quadIndex = x;
                    uint32_t splitIndex = counts[y];

                    // If the previous quad is already merged
                    if(previous.d == 0) {
                        // Use the index of the first quad
                        index = previous.y;
                        // Also save the offset
                        offset = previous.z;
                    } else {
                        index = y - 1;
                        offset = i;
                    }

                    // If it is the same quad
                    if(index == current.y)
                        continue;

                    // Adds height to the quad
                    quads[index][offset].d = y + 1;

                    // Although the quad splits, create two new quads and copy data into them
                    memcpy(&quads[y][splitIndex], &quads[y][quadIndex], sizeof(Quad));
                    memcpy(&quads[y][splitIndex + 1], &quads[y][quadIndex], sizeof(Quad));
                    counts[y] += 2;

                    // Edit the first quad so it ends correctly
                    quads[y][quadIndex].w = quads[index][offset].x;

                    // Change the next face to the merged state
                    quads[y][splitIndex].x = quads[index][offset].x;
                    quads[y][splitIndex].w = quads[index][offset].w;

                    quads[y][splitIndex].d = 0;
                    quads[y][splitIndex].y = index;
                    quads[y][splitIndex].z = offset;

                    // Save some data
                    if(quads[y][quadIndex].w == quads[y][quadIndex].x) {
                        memcpy(&quads[y][quadIndex], &quads[y][splitIndex], sizeof(Quad));
                        memcpy(&quads[y][splitIndex], &quads[y][splitIndex + 1], sizeof(Quad));
                        splitIndex--;
                        counts[y]--;
                        merged--;
                    }

                    splitIndex++;

                    // The last split quad starts at the end of the merged quad
                    quads[y][splitIndex].x = quads[index][offset].w;

                    merged++;
                    // If the overral width is over 0, means the face truly exist, so add it
                    if(quads[y][splitIndex].x == quads[y][splitIndex].w) {
                        counts[y]--;
                        merged--;
                    }

                    merged++;
                    break;
                }
            }
        }
    }
}