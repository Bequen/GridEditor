#pragma once

#include "WindowTile.h"

class PaletteTile : public WindowTile {
    public:
        PaletteTile(Scene* scene) :
        WindowTile(scene) {

        }

        void init();
        void update();
        void draw(Cursor cursor, WindowTileInfo tileInfo);
        void terminate();

        void update_palette();
        void resize_callback(uint32_t width, uint32_t height);
};