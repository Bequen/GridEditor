#pragma once

#include "Editor/Camera.h"
#include "System/Scene.h"
#include "Rendering/RenderingPipeline.h"
#include "Editor/Ray.h"
#include "System/Window.h"
#include "Editor/Color.h"
#include "PerformanceMonitor.h"
#include "Editor/FloodFillSelect.h"
#include "WindowTile.h"
#include "Editor/Input.h"
#include "Editor/CameraController.h"

#define STATE_NONE          0x0000
#define STATE_PRESS         0x0001
#define STATE_HOVER         0x0002
#define STATE_RELEASE       0x0003

#define RECTANGLE_CUBE      0x0000
#define RECTANGLE_LINE      0x0001
#define RECTANGLE_CIRCLE    0x0002

#define DRAW_MODE_BRUSH     0x0000
#define DRAW_MODE_SHAPE     0x0001

#define MAX_LIGHT_COUNT     32

#define CAMERA_MODE_PERSPECTIVE     0x0000
#define CAMERA_MODE_ORTHOGRAPHIC    0x0001

class Viewport : public WindowTile {
    public:
        bool edit;
        int32_t selectedGrid;

        Window window;

        CameraController camera;
        RenderingPipeline render;
        Input input;

        float rotationSpeed = 50.0f;
        float panSpeed = 10.0f;
        double* deltaTime;

        uint32_t drawing;
        uint32_t extruding;
        glm::vec3 shapeStart;
        glm::vec3 shapeEnd;

        uint32_t rectangle;
        glm::vec3* extrudeSelect;
        uint32_t extrudeIndex;

        uint32_t polygonMode;

        uint32_t drawMode;
        uint32_t editMode;

        uint32_t undoState;
        uint32_t redoState;
        uint32_t isEditMode;

        glm::vec3 camDirection;
        glm::vec3 camOrigin;
        float camOffset;

        Framebuffer framebuffer;
        uint32_t renderQuad;

        PerformanceStat* stat;
        _Grid tempGrid;

        FloodFillSelect floodFillSelection;

        Viewport(Scene* scene, Window window, double* deltaTime) :
        WindowTile(scene), window(window), deltaTime(deltaTime) {
            assert_msg(deltaTime, "Invalid delta time");
            assert_msg(scene, "Scene assigned to Viewport tile is not initialized");
        }

        void init();
        void init_framebuffer();
        void init_profiler();

        void update();
        void terminate();

        void draw(Cursor cursor, WindowTileInfo tileInfo);

        void extrude(glm::vec3 position, glm::vec3 normal);
        void flood_fill(glm::vec3 position, glm::vec3 normal, int8_t brush);
        void extrude(int32_t height);
        glm::vec3 ray_cast(Ray ray);

        void update_grid(_Grid grid);
        void update_cache();

        void undo();
        void redo();

        void solve_input();
        void solve_voxel_placing(Cursor cursor);
        void solve_rectangle(Grid<int8_t>* grid, glm::vec3 start, glm::vec3 end);
        void solve_rectangle(_Grid* grid, glm::vec3 start, glm::vec3 end);

        void resize_callback(uint32_t width, uint32_t height);

        void change_grid(int32_t index);

        void enter_edit_mode();
};