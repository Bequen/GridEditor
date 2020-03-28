#pragma once

#include <cstdint>

#include "System/Scene.h"
#include "WindowTile.h"

class ObjectExplorerTile : public WindowTile {
    public:
        ObjectExplorerTile(Scene* scene) :
        WindowTile(scene) {

        }

        void init();
        void update();
        void draw(Cursor cursor, WindowTileInfo tileInfo);
        void terminate();

        void refresh();

        void resize_callback(uint32_t width, uint32_t height);
};