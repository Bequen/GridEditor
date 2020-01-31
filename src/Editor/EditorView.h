#pragma once

#include <cstdint>
#include "WindowTile.h"

#define EDITOR_WINDOW_STATE_NONE 0x0000
#define EDITOR_WINDOW_STATE_RESIZE 0x0001
#define EDITOR_WINDOW_STATE_SPLIT 0x0002

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

        float width;

        uint32_t state;

        WindowTile* tile;
        WindowTileInfo tileInfo;

        void init();
        void init(uint32_t bufferSize);

        void assign(WindowTile* tile);

        void update(Cursor cursor, float x, float y, float height, uint32_t program, uint32_t flow, uint32_t winWidth, uint32_t winHeight);

        void terminate();

        void resize_callback(uint32_t width, uint32_t height);
};

