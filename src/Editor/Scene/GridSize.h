#pragma once

#include <cstdint>

#define DEFAULT_SELECTION_SIZE 256
#define SUBGRID_SIZE 8

const uint32_t SUBGRID_MAX = SUBGRID_SIZE * SUBGRID_SIZE * SUBGRID_SIZE;

#if (SUBGRID_SIZE == 8)
    typedef uint8_t GridRowType;
    const uint32_t GRID_ROW_MAX = UINT8_MAX;
#elif SUBGRID_SIZE == 16
    typedef uint16_t GridRowType;
    const uint32_t GRID_ROW_MAX = UINT16_MAX;
#elif SUBGRID_SIZE == 32
    typedef uint32_t GridRowType;
    const uint32_t GRID_ROW_MAX = UINT32_MAX;
#elif SUBGRID_SIZE == 64
    typedef uint64_t GridRowType;
    const uint32_t GRID_ROW_MAX = UINT64_MAX;
#else
#error "Invalid subgrid size, can be 8, 16, 32 or 64";
#endif
