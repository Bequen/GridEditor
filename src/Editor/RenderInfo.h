#pragma once

#include <cstdint>

struct RenderInfo {
    uint32_t voxelProgram;
    uint32_t voxelSelectedProgram;
    uint32_t quadProgram;
    uint32_t skyProgram;
    uint32_t boxProgram;
    uint32_t spriteProgram;

    uint32_t voxelVAO;
    uint32_t quadVAO;

    uint32_t currentProgram;
};