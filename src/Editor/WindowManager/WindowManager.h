#pragma once

#include <cstdint>

#include "_WindowTile.h"

/**
 * @brief  Window manager handles each window tile
 * @note   
 * @retval None
 */
class WindowManager {
    public:
        _WindowTile tiles;

        void init();
        void update();
        void terminate();

        void draw();
        void resize_callback();
        void refresh();
};