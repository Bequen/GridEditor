#pragma once

#include "Editor/Scene/GridSize.h"

/* BitGrid is grid of bits to save space and performance */
struct BitGrid {
    public:
        GridRowType* grid;
    
        BitGrid();
};