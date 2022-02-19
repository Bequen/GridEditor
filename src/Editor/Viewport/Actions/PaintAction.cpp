#include "PaintAction.h"

#include "Editor/Scene/SceneGrid.h"

#include <algorithm>

PaintAction::PaintAction(SceneGrid* grid, PaintCell cell) :
Action(ActionTypePaint), grid(grid), cells(std::vector<PaintCell>()) {
    cells.push_back(cell);
}

void
PaintAction::redo() {
    for(uint32_t i = 0; i < cells.size(); i++) {
        PaintCell cell = cells.at(i);
        grid->set(cell.index, cell.newValue);
        cells.pop_back();
    }
}

void
PaintAction::undo() {
    for(uint32_t i = cells.size() - 1; i >= 0; i++) {
        PaintCell cell = cells.at(i);
        grid->set(cell.index, cell.prevValue);
    }
}

bool
PaintAction::merge(Action* action) {
    PaintAction* old = static_cast<PaintAction*>(action);

    this->cells.insert(std::end(this->cells), std::begin(old->cells), std::end(old->cells));
    return true;
}