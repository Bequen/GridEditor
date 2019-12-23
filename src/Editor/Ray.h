#pragma once

#include <cstdint>

#include "Camera.h"
#include "System/Window.h"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;

    void create_camera_ray(Window window, Camera camera);
};