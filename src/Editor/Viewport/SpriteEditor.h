#pragma once

#include "ViewportEditor.h"

#include "System/Voxels/Grid.h"
#include "Selection.h"
#include "Shape.h"
#include "RayHit.h"
#include "Editor/Ray.h"



class SpriteEditor : public ViewportEditor {
    public:
        SceneSprite* sprite;
        SceneSprite tempSprite;

        Scene* scene;
        ViewportInfo* viewport;

        DrawMode drawMode;
        BrushMode brushMode;
        ShapeMode shapeMode;
        SelectMode selectMode;

        Selection selection;
        Shape shape;
        glm::vec3 shapeEndTemp;

        bool requireUpdate;
        bool isDrawing;
        uint32_t brushColor;

        void assign(const SceneObject* sceneObject, Scene* scene, ViewportInfo* viewport) override;

        void init() override;
        void update(RenderInfo renderInfo) override;
        void terminate() override;

        void refresh_callback();
        void resize_callback();

        void undo() override;
        void redo() override;

        void side_bar() override;
        void menu_bar() override;
        void tool_bar() override;

        void draw_sprite(RenderInfo renderInfo, const SceneSprite* sprite);

        void solve_painting();
        void solve_shape(SceneSprite* sprite, glm::vec3 start, glm::vec3 end);
};