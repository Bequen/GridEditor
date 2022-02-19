#pragma once

#include "Action.h"

#include <vector>
#include <cstdint>

struct SceneGrid;

struct PaintCell {
    uint32_t index;

    uint8_t newValue;
    uint8_t prevValue;
};

struct PaintAction : public Action {
    public:
        std::vector<PaintCell> cells;
        SceneGrid* grid;

    public:
        PaintAction(SceneGrid* grid, PaintCell cell);

        void
        redo() override;

        void
        undo() override;

        bool
        merge(Action* action) override;
};