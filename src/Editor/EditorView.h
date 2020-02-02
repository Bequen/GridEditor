#pragma once

#include <cstdint>
#include "WindowTile.h"
#include "Keyboard.h"

#define EDITOR_WINDOW_STATE_NONE 0x0000
#define EDITOR_WINDOW_STATE_RESIZE 0x0001
#define EDITOR_WINDOW_STATE_SPLIT 0x0002
#define EDITOR_WINDOW_STATE_SUBDIVIDE   0x0003

#define EDITOR_WINDOW_FLOW_X 0x0000
#define EDITOR_WINDOW_FLOW_Y 0x0001

/**
 * @brief  Window node used inside of the editor
 * @note   
 * @retval None
 */
class EditorView {
    public:
        uint32_t id;
        static uint32_t lastId;

        EditorView* children;
        uint32_t childrenCount;
        uint32_t childrenBufferSize;

        float lowestBound, highestBound;
        float crossLowestBound, crossHighestBound;
        float resizeLowestBound, resizeHighestBound;
        float width;

        uint32_t state;

        WindowTile* tile;
        WindowTileInfo tileInfo;

        EditorView() {
            children = nullptr;
            childrenCount = 0;
            childrenBufferSize = 0;
        }

        void init();
        void init(uint32_t bufferSize);

        void assign(WindowTile* tile);

        uint32_t update(Cursor cursor, Keyboard keyboard, float x, float y, float height, uint32_t program, uint32_t flow, uint32_t winWidth, uint32_t winHeight);

        uint32_t update_test(Cursor cursor, Keyboard keyboard, float x, float y, float height, uint32_t program, uint32_t flow, uint32_t winWidth, uint32_t winHeight);

        float find_bound_highest(uint32_t flow, float initial);
        float find_bound_lowest(uint32_t flow);

        void terminate();

        void resize_callback(uint32_t width, uint32_t height);
};

