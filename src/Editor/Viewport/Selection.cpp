#include "Selection.h"

Selection::Selection() {
    grid = BitGrid();
}

Selection::Selection(uint32_t size) {
    grid = BitGrid();
}

void
Selection::allocate(uint32_t size) {
    /* selection = new GridRowType[size * size]; */
}