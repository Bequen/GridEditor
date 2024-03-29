#include "CameraController.h" 

#include "Rendering/RenderLib.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <avg/Debug.h>

// TODO Split into multiple camera modes like:
//          - Fly camera
//          - Focus camera
//          - etc.
void CameraController::init() {
    // Set some initial values
    // TODO Make these adjustable
    panSpeed = 10.0f;
    rotationSpeed = 100.0f;
    direction = glm::normalize(glm::vec3(0.0f, 1.0f, -1.0f));

    // Set the controlling elements of our camera
    origin = glm::vec3(16.0f, 16.0f, 0.0f);
    offset = 20.0f;
    mode = CAMERA_MODE_PERSPECTIVE;
    width = 720;
    height = 480;

    flags = 0;
    flags = CAMERA_ALLOW_PANNING | CAMERA_ALLOW_ZOOMING | CAMERA_ALLOW_ROTATION;

    cameraBuffer = RenderLib::create_buffer_stream(GL_UNIFORM_BUFFER, sizeof(Camera), nullptr);
    camera = (Camera*)RenderLib::map_buffer_range(cameraBuffer, GL_UNIFORM_BUFFER, 0, sizeof(Camera));
    RenderLib::buffer_binding_range(cameraBuffer, 0, 0, sizeof(Camera));
    camera->projection = glm::perspective(glm::radians(45.0f), 720.0f / (480.0f), 0.1f, 100.0f);
    camera->view = glm::lookAt(origin + (-direction * offset), origin, glm::vec3(0.0f, 0.0f, 1.0f));
}

void CameraController::update() {
    // Camera Panning
    if((flags & CAMERA_ALLOW_PANNING) && Input.get(GLFW_MOUSE_BUTTON_3) == KEY_STATE_HELD && Input.get(GLFW_KEY_LEFT_SHIFT) == KEY_STATE_HELD) {
        origin += (float)Input.mouseDeltaY * panSpeed * glm::normalize(glm::vec3(camera->view[0][1], camera->view[1][1], camera->view[2][1]));
        origin += (float)Input.mouseDeltaX * panSpeed * glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0]));
    } 
    // Camera Zooming
    else if(flags & CAMERA_ALLOW_ZOOMING && Input.get(GLFW_MOUSE_BUTTON_3) && Input.get(GLFW_KEY_LEFT_CONTROL)) {
        offset += Input.mouseDeltaY * 10.0f;
    } 
    // Camera Rotation
    else if(flags & CAMERA_ALLOW_ROTATION && Input.get(GLFW_MOUSE_BUTTON_3)) {
        direction = glm::rotate(direction, (float)Input.mouseDeltaX * rotationSpeed * (float)Input.deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
        direction = glm::rotate(direction, (float)Input.mouseDeltaY * rotationSpeed * (float)Input.deltaTime, -glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0])));
        if(mode == CAMERA_MODE_ORTHOGRAPHIC)
            set_mode(CAMERA_MODE_PERSPECTIVE);
    }

    camera->view = glm::lookAt(origin + (-direction * offset), origin, glm::vec3(0.0f, 0.0f, 1.0f));
    if(mode == CAMERA_MODE_ORTHOGRAPHIC) {
        camera->projection = glm::ortho(-offset / 2.0f * aspect, offset / 2.0f * aspect, -offset / 2.0f, offset / 2.0f, 0.1f, 1000.0f);
    }
}

void CameraController::terminate() {
    RenderLib::remove_buffer(cameraBuffer);
}

void CameraController::resize_callback(uint32_t width, uint32_t height) {
    if(width == 0 || height == 0)
        return;

    this->width = width;
    this->height = height;
    MESSAGE("Resizing camera");
    aspect = (float)width / (float)height;
    if(mode == CAMERA_MODE_PERSPECTIVE)
        camera->projection = glm::perspective(glm::radians(65.0f), aspect, 0.1f, 1000.0f);
    else {
        ERROR("Orthographics");
        //camera->projection = glm::ortho(-width * , width, -height, height);
        //camera->projection = glm::perspective(glm::radians(65.0f), aspect, 0.1f, 1000.0f);
    }
}

Ray CameraController::create_ray(glm::vec3 cursor) {
    Ray result;
    glm::vec4 mouse = glm::vec4(cursor.x, cursor.y, 0.0f, 1.0f);
    mouse.z = -1.0f;

    if(mode == CAMERA_MODE_PERSPECTIVE) {
        glm::vec4 eye = glm::inverse(camera->projection) * (mouse);
        eye.z = -1.0f;

        eye.w = 0.0f;

        eye = glm::inverse(camera->view) * eye;
        eye = glm::normalize(eye);

        result.origin = (origin + (-direction * offset));
        result.direction = glm::vec3(eye);
    } else {
        glm::vec4 eye = glm::inverse(camera->projection) * (mouse);
        eye.z = 0.0f;
        eye.w = 0.0f;
        eye = glm::inverse(camera->view) * eye;

        result.origin = (origin + (-direction * offset)) + glm::vec3(eye);
        result.direction = direction;
    }

    return result;
}

void CameraController::set_mode(uint32_t mode) {
    
    switch(mode) {
        case CAMERA_MODE_ORTHOGRAPHIC: {
            this->mode = mode;
            camera->projection = glm::ortho(-offset / 2.0f, offset / 2.0f, -offset / 2.0f, offset / 2.0f, 0.1f, 1000.0f);
            break;
        } case CAMERA_MODE_PERSPECTIVE: {
            this->mode = mode;
            camera->projection = glm::perspective(glm::radians(45.0f), (float)Input.windowWidth / (float)(Input.windowHeight), 0.1f, 100.0f);
            break;
        } default: {
            ERROR("No such camera mode");
        }
    }
}

glm::vec3 CameraController::up() {
    return glm::normalize(camera->view[1]);
}