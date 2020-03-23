#include "CameraController.h" 

#include "Rendering/RenderLib.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <avg/Debug.h>

void CameraController::init() {
    panSpeed = 10.0f;
    rotationSpeed = 100.0f;
    direction = glm::normalize(glm::vec3(0.0f, 1.0f, -1.0f));

    origin = glm::vec3(16.0f, 16.0f, 0.0f);
    offset = 20.0f;
    mode = CAMERA_MODE_PERSPECTIVE;

    cameraBuffer = RenderLib::create_buffer_stream(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr);
    camera = (Camera*)RenderLib::map_buffer_range(cameraBuffer, GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2);
    RenderLib::buffer_binding_range(cameraBuffer, 0, 0, sizeof(glm::mat4) * 2);
    camera->projection = glm::perspective(glm::radians(45.0f), 720.0f / (480.0f), 0.1f, 100.0f);
    camera->view = glm::lookAt(origin + (-direction * offset), origin, glm::vec3(0.0f, 0.0f, 1.0f));
}

void CameraController::update(Input input) {
    // Camera Panning
    if(input.get(GLFW_MOUSE_BUTTON_3) && input.get(GLFW_KEY_LEFT_SHIFT)) {
        origin += (float)input.mouseDeltaY * panSpeed * glm::normalize(glm::vec3(camera->view[0][1], camera->view[1][1], camera->view[2][1]));
        origin += (float)input.mouseDeltaX * panSpeed * glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0]));
    } 
    // Camera Zooming
    else if(input.get(GLFW_MOUSE_BUTTON_3) && input.get(GLFW_KEY_LEFT_CONTROL)) {
        offset += input.mouseDeltaY * 10.0f;
    } 
    // Camera Rotation
    else if(input.get(GLFW_MOUSE_BUTTON_3)) {
        direction = glm::rotate(direction, (float)input.mouseDeltaX * rotationSpeed * (float)input.deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
        direction = glm::rotate(direction, (float)input.mouseDeltaY * rotationSpeed * (float)input.deltaTime, -glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0])));
        if(mode == CAMERA_MODE_ORTHOGRAPHIC)
            set_mode(CAMERA_MODE_PERSPECTIVE);
    }

    camera->view = glm::lookAt(origin + (-direction * offset), origin, glm::vec3(0.0f, 0.0f, 1.0f));
    if(mode == CAMERA_MODE_ORTHOGRAPHIC) {
        camera->projection = glm::ortho(-offset / 2.0f, offset / 2.0f, -offset / 2.0f, offset / 2.0f, 0.1f, 1000.0f);
    }
}

void CameraController::terminate() {
    RenderLib::remove_buffer(cameraBuffer);
}

void CameraController::resize_callback(uint32_t width, uint32_t height) {

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
            camera->projection = glm::perspective(glm::radians(45.0f), 720.0f / (480.0f), 0.1f, 100.0f);
            break;
        } default: {
            ERROR("No such camera mode");
        }
    }
}