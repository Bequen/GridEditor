#pragma once

#include "Editor/Camera.h"
#include "System/Scene.h"
#include "Rendering/RenderingPipeline.h"
#include "Editor/Ray.h"
#include "System/Window.h"
#include "Editor/Color.h"
#include "PerformanceMonitor.h"
#include "WindowTile.h"


#define STATE_NONE          0x0000
#define STATE_PRESS         0x0001
#define STATE_HOVER         0x0002
#define STATE_RELEASE       0x0003

#define RECTANGLE_CUBE      0x0000
#define RECTANGLE_LINE      0x0001
#define RECTANGLE_CIRCLE    0x0002

#define DRAW_MODE_BRUSH 0x0000
#define DRAW_MODE_SHAPE 0x0001

#define MAX_LIGHT_COUNT 32

class Viewport : public WindowTile {
    public:
        bool edit;
        uint32_t selectedGrid;

        Camera* camera;
        uint32_t cameraBuffer;

        Window window;

        RenderingPipeline render;

        double mouseDeltaX, mouseDeltaY;
        double mouseLastX, mouseLastY;

        float rotationSpeed = 50.0f;
        float panSpeed = 10.0f;
        double* deltaTime;

        uint32_t drawing;
        glm::vec3 shapeStart;
        glm::vec3 shapeEnd;

        uint32_t rectangle;
        glm::vec3* extrudeSelect;
        uint32_t extrudeIndex;

        uint32_t polygonMode;

        uint32_t drawMode;

        uint32_t undoState;
        uint32_t redoState;

        glm::vec3 camDirection;
        glm::vec3 camOrigin;
        float camOffset;

        Framebuffer framebuffer;
        uint32_t renderQuad;

        PerformanceStat* stat;

        Viewport(Scene* scene, Window window, double* deltaTime) :
        WindowTile(scene), window(window), deltaTime(deltaTime) {
            
        }

        void init();
        void init_camera();
        void init_framebuffer();
        void init_profiler();

        void update();
        void terminate();

        void draw(Cursor cursor, WindowTileInfo tileInfo);

        void extrude(glm::vec3 position, glm::vec3 normal);
        void flood_fill(glm::vec3 position, glm::vec3 normal);
        glm::vec3 ray_cast(Ray ray);

        void update_grid(Grid<int8_t> grid);
        void update_palette();
        void update_cache();
        void update_lights();
        void update_sky_color();

        void undo();
        void redo();

        void solve_input();
        void solve_voxel_placing(Cursor cursor);
        void solve_mouse();
        void solve_camera(Cursor cursor);
        void solve_rectangle(Grid<int8_t>* grid, glm::vec3 start, glm::vec3 end);

        void resize_callback(uint32_t width, uint32_t height);
};