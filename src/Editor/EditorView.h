#pragma once

#include <cstdint>
#include "Editor/Tiles/WindowTile.h"

#define EDITOR_WINDOW_STATE_NONE        0x0000
#define EDITOR_WINDOW_STATE_RESIZE      0x0001
#define EDITOR_WINDOW_STATE_SPLIT       0x0002
#define EDITOR_WINDOW_STATE_SUBDIVIDE   0x0003

#define EDITOR_WINDOW_FLOW_X            0x0000
#define EDITOR_WINDOW_FLOW_Y            0x0001

#define MENUBAR_HEIGHT                  19

#define RESIZE_LINE_WIDTH               5

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

        float width, height;

        uint32_t state;

        WindowTile* tile;
        // Tile info stores information about the tile that are used directly for rendering
        WindowTileInfo tileInfo;

        EditorView() {
            children = nullptr;
            childrenCount = 0;
            childrenBufferSize = 0;
        }

        void init();
        void init(uint32_t bufferSize);

        void assign(WindowTile* tile);

        uint32_t update(float x, float y, float height, uint32_t flow);

        void draw(float x, float y, float height, uint32_t flow);

        float find_bound_highest(uint32_t flow, float initial);
        float find_bound_lowest(uint32_t flow);

        void terminate();

        void resize_callback(uint32_t width, uint32_t height);
        
        void resize_buffer(uint32_t newBufferSize);

        void refresh();

        void drop_window(EditorView view, uint32_t flow);
        void insert_window(EditorView window, uint32_t index);
};

