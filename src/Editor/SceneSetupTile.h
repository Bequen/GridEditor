#pragma once

#include <cstdint>

#include "WindowTile.h"

class SceneSetupTile : public WindowTile {
    public:
        SceneSetupTile(Scene* scene) :
        WindowTile(scene) {

        }

        void init();
        void update();
        void terminate();

        void draw(Cursor cursor, WindowTileInfo tileInfo);
        void resize_callback(uint32_t width, uint32_t height);

        void update_lights();
};