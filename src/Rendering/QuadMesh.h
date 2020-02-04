#pragma once

#include <cstdint>

#include "QuadBuffer.h"


/**
 * @brief  Stores quad information for single grid
 * @note   
 * @retval None
 */
struct QuadMesh {
    // Quad buffer for each axis { X, Y, Z }
    QuadBuffer* buffers[3];

    // Scale of the grid
    uint32_t width, height, depth;

    QuadMesh() :
    width(0), height(0), depth(0) {

    }

    QuadMesh(uint32_t width, uint32_t height, uint32_t depth) :
    width(width), height(height), depth(depth) {
        buffers[0] = new QuadBuffer[32];
        buffers[1] = new QuadBuffer[32];
        buffers[2] = new QuadBuffer[32];

        for(uint32_t i = 0; i < height; i++) {
            buffers[0][i] = QuadBuffer(width);
            buffers[1][i] = QuadBuffer(depth);
            buffers[2][i] = QuadBuffer(height);
        }
    }

    void cleanup() {
        for(uint32_t z = 0; z < 32; z++) {
            for(uint32_t i = 0; i < 3; i++) {
                delete [] buffers[i][z].counts[0];
                delete [] buffers[i][z].counts[1];

                for(uint32_t x = 0; x < width; x++) {
                    delete [] buffers[i][z].quads[0][x];
                    delete [] buffers[i][z].quads[1][x];
                }
            }
        }
    }
};