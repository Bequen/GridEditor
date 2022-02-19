#include "SubGrid.h"

#include <glad/glad.h>

SubGrid::SubGrid() {
    texture = VoxelGrid(SUBGRID_SIZE);
    colors = Grid3D<uint8_t>(SUBGRID_SIZE);
    grid = BitGrid();
    selection = Selection();

    drawCmd.indices = new int32_t[SUBGRID_MAX];
    drawCmd.indiceCount = 0;

    drawCmd.indices[0] = 15;
    drawCmd.indiceCount = 1;

    drawCmd.buffer = RenderLib::create_buffer_dynamic(ARRAY_BUFFER, sizeof(uint32_t) * SUBGRID_MAX, drawCmd.indices);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);
}

void
SubGrid::select(uint32_t index) {
    uint32_t flatIndex = index % (SUBGRID_SIZE * SUBGRID_SIZE);
    if(this->grid.grid[(index / (SUBGRID_SIZE * SUBGRID_SIZE)) * SUBGRID_SIZE + (flatIndex / SUBGRID_SIZE)] & 1 << (flatIndex % SUBGRID_SIZE))
        this->selection.grid.grid[(index / (SUBGRID_SIZE * SUBGRID_SIZE)) * SUBGRID_SIZE + (flatIndex / SUBGRID_SIZE)] |= 1 << (flatIndex % SUBGRID_SIZE);
}

bool
SubGrid::is_selected(uint32_t index) {
    uint32_t flatIndex = index % (SUBGRID_SIZE * SUBGRID_SIZE);
    if(this->selection.grid.grid[(index / (SUBGRID_SIZE * SUBGRID_SIZE)) * SUBGRID_SIZE + (flatIndex / SUBGRID_SIZE)] & 1 << (flatIndex % SUBGRID_SIZE)) {
        return true;
    } else {
        return false;
    }
}

bool
SubGrid::is_any_selected() {
    for(uint32_t x = 0; x < SUBGRID_SIZE * SUBGRID_SIZE; x++) {
        for(uint32_t y = 0; y < sizeof(GridRowType) * 8; y++) {
            if(selection.grid.grid[x] & (1 << y)) {
                return true;
            }
        }
    } return false;
}

void
SubGrid::set(uint32_t index, uint8_t value) {
    if(index >= 8*8*8)
        return;

    this->colors.buffer[index] = value;

    uint32_t flatIndex = index % (SUBGRID_SIZE * SUBGRID_SIZE);

    if(value != 0) {
        if(!(this->grid.grid[(index / (SUBGRID_SIZE * SUBGRID_SIZE)) * SUBGRID_SIZE + (flatIndex / SUBGRID_SIZE)] & (1 << (flatIndex % SUBGRID_SIZE)))) {
            //MESSAGE("Setting on index: " << index << " << at indice: " << this->drawCmd.indiceCount);
            //this->drawCmd.indices[this->drawCmd.indiceCount++] = index;
            //update_draw_cmd(this->drawCmd.indiceCount - 1, this->drawCmd.indiceCount);
        }
        MESSAGE("Setting on " << index);
        this->grid.grid[(index / (SUBGRID_SIZE * SUBGRID_SIZE)) * SUBGRID_SIZE + (flatIndex / SUBGRID_SIZE)] |= 1 << (flatIndex % SUBGRID_SIZE);
    } else {
        this->grid.grid[(index / (SUBGRID_SIZE * SUBGRID_SIZE)) * SUBGRID_SIZE + (flatIndex / SUBGRID_SIZE)] ^= 1 << (flatIndex % SUBGRID_SIZE);
    }
}

bool
SubGrid::is_set(uint32_t index) {
    uint32_t flatIndex = index % (SUBGRID_SIZE * SUBGRID_SIZE);
    if(this->grid.grid[(index / (SUBGRID_SIZE * SUBGRID_SIZE)) * SUBGRID_SIZE + (flatIndex / SUBGRID_SIZE)] & 1 << (flatIndex % SUBGRID_SIZE)) {
        return true;
    } else {
        return false;
    }
}


void
SubGrid::dup_buffers() {
    colors.dup_buffer();
    GridRowType* temp = new GridRowType[SUBGRID_SIZE * SUBGRID_SIZE];
    memcpy(temp, grid.grid, SUBGRID_SIZE * SUBGRID_SIZE * sizeof(GridRowType));
    grid.grid = temp;
}
