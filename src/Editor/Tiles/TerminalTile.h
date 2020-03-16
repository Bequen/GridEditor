#pragma once

#include "WindowTile.h"

class TerminalTile : public WindowTile {
    public:
        char* buffer;
        uint32_t bufferSize;

        TerminalTile(Scene* scene) :
        WindowTile(scene) {

        }

        void init();
        void update();
        void draw(Cursor cursor, WindowTileInfo tileInfo);
        void terminate();

        void solve_command();
        void remove_border(char* str);

        void update_palette();
        void resize_callback(uint32_t width, uint32_t height);
};