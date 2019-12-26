#include "Editor.h"

#include "Rendering/RenderLib.h"
#include <avg/Debug.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Ray.h"
#include "Rendering/TextureLib.h"
#include "Rendering/ShaderLib.h"

void Editor::init() {
    grid = Grid<int8_t>(32);
    render.init(&grid);

    uint32_t cameraBuffer = RenderLib::create_buffer_stream(0, sizeof(glm::mat4) * 2, nullptr);
    camera = (Camera*)RenderLib::map_buffer_stream(0, cameraBuffer, 0, sizeof(glm::mat4) * 2);

    RenderLib::buffer_binding_range(cameraBuffer, 0, 0, sizeof(glm::mat4) * 2);

    camera->projection = glm::perspective(glm::radians(45.0f), 720.0f / 480.0f, 0.1f, 1000.0f);

    drawing = false;

    grid.set(0, 2);
    grid.set(1, 2);
    grid.set(2, 2);

    palette = new RGB[256];
    RGB col; col.r = 255; col.g = 0; col.b = 0;
    palette[0] = col;
    col.b = 255;
    col.r = 0;
    palette[1] = col;
    col.g = 255;
    palette[2] = col;
    palette[3] = col;

    gridTexture = TextureLib::create_texture_3d(32, 32, 32, grid.grid);
    paletteTexture = TextureLib::create_texture_1d(256, GL_RGB, GL_RGB, palette);

    panSpeed = 10.0f;
    rotationSpeed = 50.0f;
    origin = glm::vec3(0.0f);
    camDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    camPosition = glm::vec3(0.0f, 0.0f, -10.0f);
    camOrigin = glm::vec3(0.0f, 0.0f, 0.0f);
    camOffset = 10.0f;
    camera->view = glm::translate(glm::mat4(1.0f), camPosition);
    placeDelay = 0.1f;
    //camera->view = glm::lookAt(camPosition, camPosition + camDirection, glm::vec3(0.0f, 1.0f, 0.0f));

    rotationMode = 0;

    update_palette();
    update_grid();
}

void Editor::update() {
    solve_mouse();

    std::cout << "\33[2KFPS: " << 1000.0f / (float)*deltaTime << "\r";
    std::flush(std::cout);

    /* if(window.is_key_down(GLFW_KEY_W)) {
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
    } */

    solve_camera();

    solve_voxel_placing();

    RenderLib::draw_voxel(render.shader, /* (camOrigin + (-camDirection * camOffset)) + ray.direction * 20.0f */glm::vec3(0.0f, 0.0f, 0.0f));

    render.update();
}

void Editor::terminate() {

}

void Editor::solve_voxel_placing() {
    Ray ray;
    ray.create_camera_ray(window, *camera);
    float step = 0.1f;

    if(!drawing && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1) && window.is_key_down(GLFW_KEY_LEFT_SHIFT)) {
        drawing = true;
        float distance = 0.0f;

        while(distance < 100.0f) {
            distance += step;
            glm::vec3 point = (camOrigin + (-camDirection * camOffset)) + ray.direction * distance;

            if(grid.point_intersection(point)) {
                if(grid.get(point) > 0) {
                    lineStart = (camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step);
                    break;
                }
            } else {
                if(grid.point_intersection((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step))) {
                    lineStart = (camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step);
                    break;
                }
            }
        }
    } else if(drawing && !window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
        drawing = false;
        float distance = 0.0f;
        step = 0.01f;
        while(distance < 100.0f) {
            distance += step;
            glm::vec3 point = (camOrigin + (-camDirection * camOffset)) + ray.direction * distance;

            if(grid.point_intersection(point)) {
                if(grid.get(point) > 0) {
                    lineEnd = (camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step);
                    break;
                }
            } else {
                if(grid.point_intersection((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step))) {
                    lineEnd = (camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step);
                    break;
                }
            }
        }

        distance = 0.0f;
        glm::vec3 lineDir = (lineEnd - lineStart);

        while(distance < glm::length(lineDir)) {
            distance += step;
            glm::vec3 point = lineStart + glm::normalize(lineDir) * distance;

            grid.set(point, 1);
        }
    }

    else if(!drawing && glfwGetTime() > lastPlace + placeDelay) {
        float distance = 0.0f;
        while(distance < 100.0f) {
            distance += step;
            glm::vec3 point = (camOrigin + (-camDirection * camOffset)) + ray.direction * distance;

            if(grid.point_intersection(point)) {
                if(grid.get(point) > 0) {
                    if(window.is_key_down(GLFW_KEY_LEFT_CONTROL) && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
                        grid.set(point, 0);
                        lastPlace = glfwGetTime();
                    } else if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
                        grid.set((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step), 1);
                        lastPlace = glfwGetTime();
                    }
                    update_grid();
                    break;
                }
            } else {
                if(grid.point_intersection((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step))) {
                    if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1) && !window.is_key_down(GLFW_KEY_LEFT_CONTROL)) {
                        grid.set((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step), 1);
                        lastPlace = glfwGetTime();
                    }
                    update_grid();
                    break;
                }
            }
        }
    }
}

void Editor::solve_mouse() {
    glm::vec3 cursor = window.get_normalized_cursor_pos();
    mouseDeltaX = mouseLastX - cursor.x;
    mouseDeltaY = mouseLastY - cursor.y;

    mouseLastX = cursor.x;
    mouseLastY = cursor.y;
}

void Editor::update_grid() {
    TextureLib::update_texture_3d(gridTexture, 32, 32, 32, grid.grid);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, gridTexture);
    ShaderLib::uniform_int32(render.shader, "grid", 0);
}

void Editor::update_palette() {
    TextureLib::update_texture_1d(paletteTexture, 256, palette);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, paletteTexture);
    ShaderLib::uniform_int32(render.shader, "palette", 1);
}

void Editor::solve_camera() {
    // Camera Panning
    if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_3) && window.is_key_down(GLFW_KEY_LEFT_SHIFT)) {
        camOrigin += (float)mouseDeltaY * panSpeed * glm::normalize(glm::vec3(camera->view[0][1], camera->view[1][1], camera->view[2][1]));
        camOrigin += (float)mouseDeltaX * panSpeed * glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0]));
    } 
    // Camera Zooming
    else if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_3) && window.is_key_down(GLFW_KEY_LEFT_CONTROL)) {
        camOffset += mouseDeltaY * 10.0f;
    } 
    // Camera Rotation
    else if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_3)) {
        camDirection = glm::rotate(camDirection, (float)mouseDeltaX * rotationSpeed * (float)*deltaTime, glm::vec3(0.0f, 1.0f, 0.0f));
        camDirection = glm::rotate(camDirection, (float)mouseDeltaY * rotationSpeed * (float)*deltaTime, -glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0])));
    }

    camera->view = glm::lookAt(camOrigin + (-camDirection * camOffset), camOrigin, glm::vec3(0.0f, 1.0f, 0.0f));
}