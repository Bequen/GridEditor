#pragma once

#include "InputManager.h"
#include "Ray.h"
#include "Camera.h"

#define CAMERA_MODE_PERSPECTIVE     0x0000
#define CAMERA_MODE_ORTHOGRAPHIC    0x0001

class CameraController {
    public:
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