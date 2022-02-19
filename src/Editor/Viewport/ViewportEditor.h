#pragma once

#include <cstdint>

#include "Editor/SceneObject.h"
#include "Editor/Scene.h"
#include "Editor/RenderInfo.h"
#include "ViewportInfo.h"

#include "Editor/Enums.h"

class ViewportEditor {
    public:
        virtual void assign(const SceneObject* sceneObject, Scene* scene, ViewportInfo* viewport) = 0;

        virtual void init() = 0;
        virtual void update(RenderInfo renderInfo) = 0;
        virtual void terminate() = 0;

        virtual void draw(RenderInfo renderInfo, WindowTileInfo tileInfo) = 0;

        virtual void refresh_callback() = 0;
        virtual void resize_callback() = 0;

        virtual void undo() = 0;
        virtual void redo() = 0;

        virtual void tool_bar() = 0;
        virtual void menu_bar() = 0;
        virtual void side_bar() = 0;
};