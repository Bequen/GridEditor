#include "Editor.h"

#include "Rendering/RenderLib.h"
#include <avg/Debug.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Ray.h"

void Editor::init() {
    grid = Grid<int8_t>(32);
    render.init(&grid);

    uint32_t cameraBuffer = RenderLib::create_buffer_stream(0, sizeof(glm::mat4) * 2, nullptr);
    camera = (Camera*)RenderLib::map_buffer_stream(0, cameraBuffer, 0, sizeof(glm::mat4) * 2);

    RenderLib::buffer_binding_range(cameraBuffer, 0, 0, sizeof(glm::mat4) * 2);

    camera->projection = glm::perspective(glm::radians(45.0f), 720.0f / 480.0f, 0.1f, 1000.0f);

    grid.set(0, 1);
    grid.set(1, 1);
    grid.set(2, 1);

    panSpeed = 10.0f;
    rotationSpeed = 50.0f;
    origin = glm::vec3(0.0f);
    camDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    camPosition = glm::vec3(0.0f, 0.0f, -10.0f);
    camOrigin = glm::vec3(0.0f, 0.0f, 0.0f);
    camOffset = 10.0f;
    camera->view = glm::translate(glm::mat4(1.0f), camPosition);
    placeDelay = 0.1;
    //camera->view = glm::lookAt(camPosition, camPosition + camDirection, glm::vec3(0.0f, 1.0f, 0.0f));

    rotationMode = 0;
}

void Editor::update() {
    glm::vec3 cursor = window.get_normalized_cursor_pos();
    mouseDeltaX = mouseLastX - cursor.x;
    mouseDeltaY = mouseLastY - cursor.y;

    mouseLastX = cursor.x;
    mouseLastY = cursor.y;

    if(window.is_key_down(GLFW_KEY_W)) {
        //camPosition = camPosition + glm::vec3(0.0f, 0.0f, 10.0f * *deltaTime);
        //camPosition += camDirection * 10.0f * (float)*deltaTime;
        //camera->view = glm::translate(camera->view, glm::vec3(0.0f, 0.0f, 10.0f * *deltaTime));
    } if(window.is_key_down(GLFW_KEY_S)) {
        //camPosition = camPosition + glm::vec3(0.0f, 0.0f, -10.0f * *deltaTime);
        //camPosition += -camDirection * 10.0f * (float)*deltaTime;
        //camera->view = glm::translate(camera->view, glm::vec3(0.0f, 0.0f, -10.0f * *deltaTime));
    } if(window.is_key_down(GLFW_KEY_A)) {
        //camPosition += -glm::cross(camDirection, glm::vec3(0.0f, 1.0f, 0.0f)) * 10.0f * (float)*deltaTime;
        //camPosition = camPosition + glm::vec3(10.0f * *deltaTime, 0.0f, 0.0f);
        //camera->view = glm::translate(camera->view, glm::vec3(10.0f * *deltaTime, 0.0f, 0.0f));
    } if(window.is_key_down(GLFW_KEY_D)) {
        //camPosition += glm::cross(camDirection, glm::vec3(0.0f, 1.0f, 0.0f)) * 10.0f * (float)*deltaTime;
        //camPosition = camPosition + glm::vec3(-10.0f * *deltaTime, 0.0f, 0.0f);
        //camera->view = glm::translate(camera->view, glm::vec3(-10.0f * *deltaTime, 0.0f, 0.0f));
    }

    /* float step = 0.1f;
    
    glm::vec3 camPos = camera->view[3]; */

    if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_3) && window.is_key_down(GLFW_KEY_LEFT_SHIFT)) {
        camOrigin += (float)mouseDeltaY * panSpeed * glm::normalize(glm::vec3(camera->view[0][1], camera->view[1][1], camera->view[2][1]));
        camOrigin += (float)mouseDeltaX * panSpeed * glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0]));
        //camera->view = glm::translate(camera->view, (float)mouseDeltaY * panSpeed * glm::normalize(glm::vec3(camera->view[0][1], camera->view[1][1], camera->view[2][1])));
        //camera->view = glm::translate(camera->view, (float)mouseDeltaX * panSpeed * glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0])));
    } else if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_3) && window.is_key_down(GLFW_KEY_LEFT_CONTROL)) {
        camOffset += mouseDeltaY * 10.0f;
    } else if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_3)) {
        //camera->view = glm::translate(camera->view, -origin);

        //camera->view = glm::rotate(camera->view, (float)mouseDeltaX * rotationSpeed * (float)*deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        //camera->view = glm::rotate(camera->view, (float)mouseDeltaY * rotationSpeed * (float)*deltaTime, -glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0])));

        camDirection = glm::rotate(camDirection, (float)mouseDeltaX * rotationSpeed * (float)*deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        camDirection = glm::rotate(camDirection, (float)mouseDeltaY * rotationSpeed * (float)*deltaTime, -glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0])));

        //camera->view = glm::translate(camera->view, origin);
    }

    camera->view = glm::lookAt(camOrigin + (-camDirection * camOffset), camOrigin, glm::vec3(0.0f, 1.0f, 0.0f));

    Ray ray;
    ray.create_camera_ray(window, *camera);


    /* camDirection = glm::rotate(camDirection, (float)*deltaTime * rotationSpeed * -(float)mouseDeltaX, glm:-glm::vec3(camera->view[3]) + ray.direction * 20.0f
    camera->view = glm::lookAt(camPosition, camPosition + camDirection, glm::vec3(0.0f, 1.0f, 0.0f));

    Ray ray;
    ray.create_camera_ray(window, *camera);

    glm::vec3 pp = camPosition;

    std::cout << pp.x << " " << pp.y << " " << pp.z << "\n";
    std::cout << (pp + ray.direction * 20.0f).x << " " << (pp + ray.direction * 20.0f).y << " " << (pp + ray.direction * 20.0f).z << "\n";
    std::cout << (camPosition + camDirection * 20.0f).x << " " << (camPosition + camDirection * 20.0f).y << " " << (camPosition + camDirection * 20.0f).z << CURSOR_UP(2) << "\r";
    std::flush(std::cout); */
    std::cout << "\33[2K" << camera->view[3].x << " " << camera->view[3].y << " " << camera->view[3].z << "\r";
    std:flush(std::cout);

    glm::vec3 camMatPos = glm::vec3(camera->view[3].x, camera->view[3].y, camera->view[3].z);

    float step = 0.1f;
    float distance = 0.0f;
    if(glfwGetTime() > lastPlace + placeDelay) {
        while(distance < 100.0f) {
            distance += step;

            std::cout << ray.origin.x << " " << ray.origin.y << " " << ray.origin.z << "\r";
            std::flush(std::cout);
            if(grid.get((camOrigin + (-camDirection * camOffset)) + ray.direction * distance) != 0) {
                if(window.is_key_down(GLFW_KEY_LEFT_CONTROL) && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
                    grid.set((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance), 0);
                    lastPlace = glfwGetTime();
                } else if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
/*                     glm::vec3 rayPos = (camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step);
                    if(rayPos.x < 0 || rayPos.y < 0 || rayPos.z < 0 ||
                        rayPos.x > grid.size || rayPos.y > grid.size || rayPos.z > grid.size) */

                    grid.set((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step), 1);
                    lastPlace = glfwGetTime();
                }
                break;
            }
        }
    }

    RenderLib::draw_voxel(render.shader, (camOrigin + (-camDirection * camOffset)) + ray.direction * 20.0f);

    render.update();
    //window.set_cursor_pos(720.0f / 2.0f, 480.0f / 2.0f);
}

void Editor::terminate() {

}