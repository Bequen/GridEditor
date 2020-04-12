#pragma once

#include "Editor/WindowManager/WindowEditor.h"

class SceneExplorerTile : public WindowEditor {
    public:
        Scene* scene;
        RenderInfo renderInfo;
        
        SceneExplorerTile(Scene* scene, RenderInfo renderInfo);

        void init();
        void update();
        void draw(WindowTileInfo tileInfo) override;
        void terminate();

        void refresh();

        void resize_callback(uint32_t width, uint32_t height);

        void tree_node(SceneObject* sceneObject);

        void update_lights();
};