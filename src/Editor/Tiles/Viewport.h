#pragma once

#include "Editor/Ray.h"
#include "Editor/Viewport/RayHit.h"
#include "Editor/Scene.h"
#include "Editor/Color.h"
#include "Editor/Camera.h"
#include "Editor/RenderInfo.h"
#include "Editor/InputManager.h"
#include "Editor/CameraController.h"

#include "Rendering/RenderingPipeline.h"
#include "System/Window.h"
#include "WindowTile.h"
#include "Editor/Viewport/Selection.h"
#include "Editor/WindowManager/WindowEditor.h"
#include "System/Voxels/GridCache.h"

#include "Editor/Viewport/VoxelGridEditor.h"

typedef uint32_t DrawMode;
typedef uint32_t ShapeMode;
typedef uint32_t BrushMode;

#define SHAPE_CUBE                  0x0000
#define SHAPE_LINE                  0x0001
#define SHAPE_CIRCLE                0x0002

#define DRAW_MODE_BRUSH             0x0000
#define DRAW_MODE_SHAPE             0x0001
#define DRAW_MODE_EXTRUDE           0x0002


#define MAX_LIGHT_COUNT             32

#define TRANSFORM_MODE_NONE         0x0000
#define TRANSFORM_MODE_TRANSLATE    0x0001
#define TRANSFORM_MODE_ROTATE       0x0002
#define TRANSFORM_MODE_SCALE        0x0003

class Viewport : public WindowEditor {
    public:
        uint32_t requireUpdate;

        DrawMode drawMode;
        BrushMode brushMode;
        BrushMode brushModeCache;
        ShapeMode shapeMode;
        uint32_t isDrawing;

        glm::vec3 shapeStart;
        glm::vec3 shapeEnd;
        glm::vec3 shapeNormal;
        uint32_t shapeNormalOffset;

        Selection selection;

        // Grid we use to draw temporarily before copying data into $selectedGrid
        Grid tempGrid;
        // Selected grid is the current grid in the array, use to paste the final result or retrieve the current grid
        Grid* selectedGrid;
        uint32_t selectIndex;

        uint32_t polygonMode;

        uint32_t undoState;
        uint32_t redoState;

        uint32_t transformMode;
        float tranformDistance;
        uint32_t snapping;
        uint32_t snappingSwitch;

        Framebuffer framebuffer;
        uint32_t renderQuad;

        Scene* scene;
        RenderInfo renderInfo;

        ViewportInfo info;

        GridCache tempCache;
        GridCache* cache;
        uint32_t cacheDepth;
        uint32_t cacheSize;
        uint32_t cacheIndex;

        ViewportEditor* viewportEditor;

        Viewport(Scene* scene, RenderInfo renderInfo) :
        scene(scene), renderInfo(renderInfo) {
            
        }

        void init();
        void init_framebuffer();
        void init_profiler();

        void enter_edit_mode();

        void update();
        void terminate();
        void refresh();

        void draw(WindowTileInfo tileInfo);
        void draw_ui();
        void draw_scene_object(const SceneObject* sceneObject);

        bool select_scene_object(SceneObject* sceneObject);

        void extrude(glm::vec3 position, glm::vec3 normal);
        void flood_fill(glm::vec3 position, glm::vec3 normal, int8_t brush);
        void extrude(int32_t height);
        RayHit ray_cast(Ray ray);

        void update_grid(Grid grid);
        void update_cache();

        void undo();
        void redo();

        void solve_input();
        void solve_voxel_placing();
        void solve_rectangle(Grid* grid, glm::vec3 start, glm::vec3 end);

        void resize_callback(uint32_t width, uint32_t height);

        void change_grid(int32_t index);

        void select_grid(uint32_t index);
        void select_grid(Grid* grid);

        void draw_scene_object(const SceneObject* sceneObject, uint32_t ahoj);

        uint32_t get_index(glm::vec3 pos);
};