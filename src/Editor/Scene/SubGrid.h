#pragma once

#include <cstdint>

#include "Editor/Scene/GridSize.h"
#include "Editor/Scene/BitGrid.h"
#include "System/Grid3D.h"
#include "Editor/VoxelGrid.h"
#include "Editor/Viewport/Selection.h"

#include "Rendering/RenderLib.h"

// Used for quick, single draw call drawing
struct DrawCmd {
    int32_t* indices;
    uint32_t indiceCount;

    uint32_t buffer;
};

struct SubGrid {
    public:
        VoxelGrid texture;
        Grid3D<uint8_t> colors;
        BitGrid grid;

        Selection selection;

        DrawCmd drawCmd;

        SubGrid();

        // Updates draw command
        // From and To are indexes, not memory offset, don't do * sizeof(uint32_t);
        inline void
        update_draw_cmd(uint32_t from, uint32_t to) {
            assert_msg(from != to, "From and To cannot be same value");

            MESSAGE("Updating draw cmd from " << from << " to " << to);
            void* pointer = RenderLib::map_buffer_range(drawCmd.buffer, ARRAY_BUFFER, 0, to * sizeof(uint32_t));
            memcpy(pointer, drawCmd.indices + 0, (to - 0) * sizeof(uint32_t));
            RenderLib::unmap_buffer(ARRAY_BUFFER);
        }

        inline void select(uint32_t index);
        inline bool is_selected(uint32_t index);
        inline bool is_any_selected();

        inline void set(uint32_t index, uint8_t value);
        inline bool is_set(uint32_t index);

        /* Duplicates all the buffers, useful when you want to create
         * new grid based on some other, but want to leave the original
         * untouched */
        inline void dup_buffers();
};
