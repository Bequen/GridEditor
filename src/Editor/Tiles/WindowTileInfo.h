#pragma once

#include <cstdint>

#define WINDOW_TILE_STATE_NONE      0x0000
#define WINDOW_TILE_STATE_HOVERED   0x0001
#define WINDOW_TILE_STATE_FOCUSED   0x0002

struct WindowTileInfo {
    float x, y;
    float width, height;
};