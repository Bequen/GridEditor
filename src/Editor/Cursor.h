#pragma once

#include <cstdint>

#define MOUSE_BUTTON_STATE_NONE       0x0000
#define MOUSE_BUTTON_STATE_PRESS      0x0001
#define MOUSE_BUTTON_STATE_HOLD       0x0002

/**
 * @brief  Structure holding information about the cursor
 * @note   
 * @retval None
 */
struct Cursor {
    double cursorX, cursorY;
    double deltaX, deltaY;

    uint32_t leftButtonState, rightButtonState, middleButtonState;

    float scrollDelta;
};