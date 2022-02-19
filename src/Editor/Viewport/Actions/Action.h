#pragma once

#include <cstdint>

enum ActionType {
    ActionTypePaint
};

struct Action {
    public:
        ActionType type;

        Action(ActionType type) :
        type(type) {

        }

        virtual void
        undo() = 0;

        virtual void
        redo() = 0;

        virtual bool
        merge(Action* action) = 0;
};