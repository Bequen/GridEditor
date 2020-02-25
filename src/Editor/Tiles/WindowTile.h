#pragma once

#include "Editor/Cursor.h"
#include "System/Scene.h"
#include "WindowTileInfo.h"
#include "System/Window.h"

class WindowTile {
    public:
        WindowTileInfo tileInfo;
        Scene* scene;
        Window* window;

        WindowTile(Scene* scene) :
        scene(scene) {
            
        }

        virtual void init() = 0;
        virtual void update() = 0;
        virtual void draw(Cursor cursor, WindowTileInfo tileInfo) = 0;
        virtual void resize_callback(uint32_t width, uint32_t height) = 0;
};