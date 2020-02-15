#pragma once

#include <cstdint>

#include "_Quad.h"
#include "RenderLib.h"

/**
 * @brief  A mesh contained only from quads with only 6 possible normals
 * @note   Can be used to render voxel art. Allows for variable number of compressions.
 * @retval None
 */
struct _QuadMesh {
    _Quad* quads[2];
    uint32_t counts[2] = { 0, 0 };

    void init(uint32_t size) {
        quads[0] = { new _Quad[size] };
        quads[1] = { new _Quad[size] };
    }
};