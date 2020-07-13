#pragma once

#include "Editor/WindowManager/WindowEditor.h"

class TimelineTile : public WindowEditor {
    public:
        Scene* scene;
        RenderInfo renderInfo;
        
        TimelineTile(Scene* scene, RenderInfo renderInfo);

        void init();
        void update();
        void draw(WindowTileInfo tileInfo) override;
        void terminate();

        void refresh();

        void update_palette();
        void resize_callback(uint32_t width, uint32_t height);
};