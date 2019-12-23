#pragma once

#include "Camera.h"
#include "System/Window.h"
#include "System/Grid.h"
#include "Rendering/RenderingPipeline.h"

#define STATE_NONE 0x0000
#define STATE_PRESS 0x0001
#define STATE_HOVER 0x0002
#define STATE_RELEASE 0x0003

class Editor {
    public:
        Camera* camera;

        Window window;
        Grid<int8_t> grid = Grid<int8_t>(32);

        uint32_t program;
        uint32_t VAO;

        RenderingPipeline render;

        glm::vec3 origin;
        float distance = 0;
        glm::vec3 rotation;

        glm::vec3 camDirection;
        glm::vec3 camPosition;
        
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

        void init();
        void update();
        void terminate();

        void solve_voxel_placing();
        void solve_mouse();
};