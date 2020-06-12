#pragma once

#include "ViewportEditor.h"

#include "System/Voxels/Grid.h"
#include "Selection.h"
#include "Shape.h"
#include "RayHit.h"
#include "Editor/Scene/SceneGrid.h"
#include "Editor/Ray.h"

class VoxelGridEditor : public ViewportEditor {
    public:
        SceneGrid* grid;
        SceneGrid tempGrid;

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

        void draw(RenderInfo renderInfo, WindowTileInfo tileInfo) override;

        void refresh_callback();
        void resize_callback();

        void undo() override;
        void redo() override;

        void side_bar() override;
        void menu_bar() override;
        void tool_bar() override;

        #pragma region VOXEL EDITOR
        void update_camera();
        void solve_voxel_drawing();
        void solve_shape(SceneGrid* grid, glm::vec3 start, glm::vec3 end);
        void select_shape(SceneGrid* grid, Shape shape);


        void extrude(int32_t height);
        void flood_fill(glm::vec3 position, glm::vec3 normal);
        RayHit ray_cast(const Ray& ray);
        uint32_t get_index(glm::vec3 position);
        void update_grid(const Grid* grid);

        void draw_grid(RenderInfo renderInfo, const SceneGrid* grid);
        glm::vec3 floor_vec(glm::vec3 vec);

        void update_cache();
        #pragma endregion
};