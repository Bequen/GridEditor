#pragma once

#include <cstdint>

#include "Editor/SceneObject.h"
#include "Editor/Scene.h"
#include "Editor/RenderInfo.h"
#include "ViewportInfo.h"

typedef uint32_t DrawMode;
typedef uint32_t ShapeMode;
typedef uint32_t BrushMode;
typedef uint32_t SelectMode;

#define SHAPE_CUBE                  0x0000
#define SHAPE_LINE                  0x0001
#define SHAPE_CIRCLE                0x0002

#define DRAW_MODE_BRUSH             0x0000
#define DRAW_MODE_SHAPE             0x0001
#define DRAW_MODE_EXTRUDE           0x0002
#define DRAW_MODE_SELECTION         0x0003

#define BRUSH_MODE_ADD              0x0000
#define BRUSH_MODE_SUBSTRACT        0x0001
#define BRUSH_MODE_PAINT            0x0002

#define SELECT_MODE_BOX             0x0000

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