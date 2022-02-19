#include "BitGrid.h"

#include <cstring>

BitGrid::BitGrid() {
    grid = new GridRowType[SUBGRID_SIZE * SUBGRID_SIZE];
    memset(grid, 0, SUBGRID_SIZE * SUBGRID_SIZE * sizeof(GridRowType));
}