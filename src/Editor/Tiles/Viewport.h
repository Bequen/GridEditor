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

typedef uint32_t DrawMode;
typedef uint32_t ShapeMode;
typedef uint32_t BrushMode;

#define SHAPE_CUBE                  0x0000
#define SHAPE_LINE                  0x0001
#define SHAPE_CIRCLE                0x0002

#define DRAW_MODE_BRUSH             0x0000
#define DRAW_MODE_SHAPE             0x0001
#define DRAW_MODE_EXTRUDE           0x0002

#define BRUSH_MODE_ADD              0x0000
#define BRUSH_MODE_SUBSTRACT        0x0001
#define BRUSH_MODE_PAINT            0x0002

#define MAX_LIGHT_COUNT             32

#define CAMERA_MODE_PERSPECTIVE     0x0000
#define CAMERA_MODE_ORTHOGRAPHIC    0x0001

#define TRANSFORM_MODE_NONE         0x0000
#define TRANSFORM_MODE_TRANSLATE    0x0001
#define TRANSFORM_MODE_ROTATE       0x0002
#define TRANSFORM_MODE_SCALE        0x0003

class Viewport : public WindowEditor {
    public:
        uint32_t isEditMode;
        uint32_t requireUpdate;
        CameraController camera;

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
        WindowTileInfo tileInfo;

        Viewport(Scene* scene, RenderInfo renderInfo) :
        scene(scene), renderInfo(renderInfo) {
            
        }

        void init();
        void init_framebuffer();
        void init_profiler();

        void update();
        void terminate();
        void refresh();

        void draw(WindowTileInfo tileInfo);
        void draw_ui();

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

        void leave_edit_mode();
        void enter_edit_mode();
};