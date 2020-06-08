#pragma once

#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define KEY_STATE_NONE      0x0000
#define KEY_STATE_PRESS     0x0001
#define KEY_STATE_HELD      0x0002
#define KEY_STATE_RELEASE   0x0003

#include "System/Window.h"
#include "Editor/Tiles/WindowTileInfo.h"

struct InputKey {
    uint32_t key;
    uint32_t state;
};

// TODO Probably some sort of hashing would be cool
// TODO Also, making it anonymous class would be better
class InputManager {
    private:
        Window window;

    public:
        InputKey* keys;
        uint32_t keyCount;
        uint32_t bufferSize;

        InputKey* mouseKeys;
        uint32_t mouseKeyCount;
        uint32_t mouseKeyBufferSize;

        double mouseDeltaX; double mouseDeltaY;
        double mouseLastX; double mouseLastY;

        double lastTime;
        double deltaTime;

        int32_t windowWidth;
        int32_t windowHeight;
        bool isResized;

        /**
         * @brief  Initializes the input manager for specific number of input keys
         * @note   Added input keys can extend the limit, the preset limit should not be exceeded in non-debug builds to achieve performance
         * @param  count: Number of expected inputs
         * @retval None
         */
        void init(uint32_t count);

        // TODO Make these two into one
        void add_key(InputKey key);
        void add_mouse_key(InputKey key);

        /**
         * @brief  Updates the input keys
         * @note   
         * @retval None
         */
        void update();

        uint32_t get(uint32_t key);
        void get_mapped_cursor(WindowTileInfo tile, double* x, double* y);

        void set_cursor(uint32_t cursor);
};

extern InputManager Input;