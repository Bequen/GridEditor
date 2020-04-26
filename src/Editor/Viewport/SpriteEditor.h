#pragma once

#include "ViewportEditor.h"

#include "System/Voxels/Grid.h"
#include "Selection.h"
#include "Shape.h"
#include "RayHit.h"
#include "Editor/Ray.h"



class SpriteEditor : public ViewportEditor {
    public:
        Grid* grid;
        Grid tempGrid;

        Scene* scene;
        ViewportInfo* viewport;

        DrawMode drawMode;
        BrushMode brushMode;
        ShapeMode shapeMode;

        bool isDrawing;
        bool requireUpdate;

        void assign(const SceneObject* sceneObject, Scene* scene, ViewportInfo* viewport) override;

        void init() override;
        void update(RenderInfo renderInfo) override;
        void terminate() override;

        void refresh_callback();
        void resize_callback();

        void solve_painting();
        void update_grid(const Grid* grid);
};