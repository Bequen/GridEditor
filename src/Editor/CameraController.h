#pragma once

#include "InputManager.h"
#include "Ray.h"
#include "Camera.h"

#define CAMERA_MODE_PERSPECTIVE     0x0000
#define CAMERA_MODE_ORTHOGRAPHIC    0x0001

#define CAMERA_ALLOW_PANNING        1 << 1
#define CAMERA_ALLOW_ROTATION       1 << 2
#define CAMERA_ALLOW_ZOOMING        1 << 3

class CameraController {
    public:
        uint32_t flags;

        float panSpeed;
        float rotationSpeed;

        float offset;
        float aspect;

        glm::vec3 direction;
        glm::vec3 origin;

        uint32_t mode;

        uint32_t cameraBuffer;
        Camera* camera;

        void init();
        void update();
        void terminate();

        void resize_callback(uint32_t width, uint32_t height);

        Ray create_ray(glm::vec3 cursor);
        void set_mode(uint32_t mode);

        glm::vec3 up();
};