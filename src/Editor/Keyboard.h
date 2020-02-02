#pragma once

#include <cstdint>

#define KEY_STATE_NONE  0x0000
#define KEY_STATE_PRESS 0x0001
#define KEY_STATE_HOLD  0x0002

struct Keyboard {
    uint32_t leftControl;
    uint32_t leftShift;
    uint32_t leftAlt;
};