#include "Ray.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

void Ray::create_camera_ray(Window window, Camera camera) {
    glm::vec4 mouse = glm::vec4(window.get_normalized_cursor_pos(), 1.0f);
    mouse.z = -1.0f;

    glm::vec4 eye = glm::inverse(camera.projection) * (mouse);
    eye.z = -1.0f;  
    eye.w = 0.0f;

    eye = glm::inverse(camera.view) * eye;
    eye = glm::normalize(eye);

    this->origin = glm::vec3(camera.view[0][3], camera.view[1][3], camera.view[2][3]);
    this->direction = glm::vec3(eye);
    //this->origin = glm::vec3(camera.view[3]);
}