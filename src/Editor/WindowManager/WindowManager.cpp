#include "WindowManager.h"

#include "Editor/InputManager.h"

#include <avg/Debug.h>

void WindowManager::init() {
    // Assign default value for the main window
    _WindowTile tile;
    tile.init();
    tile.flow = TILE_FLOW_HORIZONTAL;
    tile.width = 1.0f;
    tiles = tile;
}

void WindowManager::update() {
    tiles.update(0.1f, 0.1f, 0.9f);
}

void WindowManager::draw() {
    float offset = 19.0 / Input.windowHeight;
    tiles.draw(offset, 0.0, 1.0 - offset, TILE_FLOW_HORIZONTAL, 0);
}

void WindowManager::resize_callback() {
    tiles.resize_callback();
}

void WindowManager::refresh() {
    tiles.refresh();
}
