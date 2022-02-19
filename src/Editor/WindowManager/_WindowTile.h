#pragma once

#include <cstdint>

#include "WindowEditor.h"
#include "Editor/Tiles/WindowTileInfo.h"

#define TILE_FLOW_HORIZONTAL    0x0000
#define TILE_FLOW_VERTICAL      0x0001

#define TILE_STATE_NONE         0x0000
#define TILE_STATE_RESIZE_BEGIN 0x0001
#define TILE_STATE_RESIZE       0x0002
#define TILE_STATE_RESIZE_END   0x0003

struct _WindowTile {
    static uint32_t ID;
    uint32_t id;

    float width;
    uint32_t flow;
    uint32_t state;

    _WindowTile* children;
    uint32_t childrenCount;
    uint32_t childrenBufferSize;

    WindowTileInfo tileInfo;

    WindowEditor* editors;
    uint32_t editorCount;
    uint32_t editorBufferSize;

    float resizeLowestBound;
    float resizeHighestBound;
    

    void init();
    void update(float offsetX, float offsetY, float height);
    void draw(float offsetX, float offsetY, float height, uint32_t flow, int32_t index);

    void insert_window(_WindowTile tile, uint32_t index);

    float find_lowest_bound(uint32_t flow);
    float find_highest_bound(uint32_t flow, float initial);

    void resize_callback();
    void refresh();
};
