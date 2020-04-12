#pragma once

#include "Editor/WindowManager/WindowEditor.h"

class PropertiesTile : public WindowEditor {
    public:
        Scene* scene;
        RenderInfo renderInfo;
        
        PropertiesTile(Scene* scene, RenderInfo renderInfo);

        void init();
        void update();
        void draw(WindowTileInfo tileInfo) override;
        void terminate();

        void refresh();

        void resize_callback(uint32_t width, uint32_t height);
};