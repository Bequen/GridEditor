#pragma once

#include "Camera.h"
#include "System/Window.h"
#include "System/Grid.h"
#include "Rendering/RenderingPipeline.h"
#include "Color.h"
#include "Ray.h"

#define STATE_NONE 0x0000
#define STATE_PRESS 0x0001
#define STATE_HOVER 0x0002
#define STATE_RELEASE 0x0003

#define RECTANGLE_CUBE      0x0000
#define RECTANGLE_LINE      0x0001
#define RECTANGLE_CIRCLE    0x0002

#define DRAW_MODE_BRUSH 0x0000
#define DRAW_MODE_SHAPE 0x0001

class Editor {
    public:
        Camera* camera;

        Window window;
        Grid<int8_t> grid = Grid<int8_t>(32);

        RGB32* palette;
        uint32_t colorSelected;
        uint32_t colorCache;

        uint32_t program;
        uint32_t VAO;

        RenderingPipeline render;

        glm::vec3 camDirection;
        float camOffset;
        glm::vec3 camOrigin;

        float lastPlace;
        float placeDelay;

        double mouseDeltaX, mouseDeltaY;
        double mouseLastX, mouseLastY;

        float rotationSpeed = 50.0f;
        float panSpeed = 10.0f;

        uint32_t rotationMode = 0;
        uint32_t state = 0;
        double* deltaTime;

        uint32_t gridTexture;
        uint32_t paletteTexture;

        uint32_t drawing;
        glm::vec3 shapeStart;
        glm::vec3 shapeEnd;

        uint32_t rectangle;
        glm::vec3* extrudeSelect;
        uint32_t extrudeIndex;

        uint32_t drawMode;

        void init();
        void update();
        void terminate();

        void solve_voxel_placing();
        void solve_mouse();
        void solve_camera();
        void solve_rectangle(glm::vec3 start, glm::vec3 end);

        void extrude(glm::vec3 position, glm::vec3 normal);
        void flood_fill(glm::vec3 position, glm::vec3 normal);
        glm::vec3 ray_cast(Ray ray);

        void update_grid();
        void update_palette();

        void draw_ui();
        void draw_palette();
        void draw_toolbar();

        void resize_callback(int32_t width, int32_t height);
};