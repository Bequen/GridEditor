#include "Editor.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

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

    camera->projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);
    colorSelected = 0;

    drawing = false;

    palette = new RGB32[256];
    memset(palette, 0, sizeof(RGB32) * 256);
    gridTexture = TextureLib::create_texture_3d(32, 32, 32, grid.grid);
    paletteTexture = TextureLib::create_texture_1d(256, GL_RGB, GL_RGB, palette);

    panSpeed = 10.0f;
    rotationSpeed = 100.0f;
    origin = glm::vec3(0.0f);
    camDirection = glm::vec3(0.0f, 0.0f, 1.0f);
    camPosition = glm::vec3(0.0f, 0.0f, -10.0f);
    camOrigin = glm::vec3(0.0f, 0.0f, 0.0f);
    camOffset = 10.0f;
    camera->view = glm::translate(glm::mat4(1.0f), camPosition);
    placeDelay = 0.1f;
    rectangle = 0;
    //camera->view = glm::lookAt(camPosition, camPosition + camDirection, glm::vec3(0.0f, 1.0f, 0.0f));

    rotationMode = 0;

    update_palette();
    update_grid();
}

void Editor::update() {
    solve_mouse();

    if(!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyWindowFocused()) {
        solve_camera();
        solve_voxel_placing();
    }

    RenderLib::draw_voxel(render.shader, /* (camOrigin + (-camDirection * camOffset)) + ray.direction * 20.0f */glm::vec3(0.0f, 0.0f, 0.0f));

    render.update();
    draw_ui();
}

void Editor::draw_ui() {
    draw_palette();
    draw_toolbar();
}

void Editor::draw_palette() {
    ImGui::Begin("Pallette", nullptr, ImGuiWindowFlags_NoMove);

    ImGui::BeginChild("PickerWindow", ImVec2(200.0f, 200.0f), true);
    if(ImGui::ColorPicker3("picker", &palette[colorSelected].r, ImGuiColorEditFlags_PickerHueWheel))
        update_palette();
    ImGui::EndChild();

    ImGui::BeginChild("PaletteWindow", ImVec2(0.0f, 0.0f), true);
    float buttonSize = 20.0f;
    float padding = 5.0f;

    float size = ImGui::GetWindowContentRegionWidth();
    int rowSize = (int)std::floor(size / (buttonSize + padding));
    for(uint32_t i = 0; i < 256; i++) {
        const char* label = std::to_string(i).c_str();
        if(rowSize > 0) {
            if(i % rowSize)
                ImGui::SameLine((i % rowSize) * (buttonSize + padding) + padding);
            if(ImGui::ColorButton(label, ImVec4(palette[i].r, palette[i].g, palette[i].b, 1.0f), 0, ImVec2(buttonSize, buttonSize))) {
                colorSelected = i;
                colorCache = i;
            }
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void Editor::draw_toolbar() {
    ImGui::Begin("Toolbar");

    const char* items[] = { "Cube", "Line", "Circle" };

    if (ImGui::BeginCombo("##combo", items[rectangle])) // The second parameter is the label previewed before opening the combo.
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            bool is_selected = (rectangle == n); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(items[n], is_selected))
                rectangle = n;
            if (is_selected)
                ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
        }
        ImGui::EndCombo();
    }

    ImGui::End();
}

void Editor::terminate() {

}

void Editor::solve_voxel_placing() {
    Ray ray;
    ray.create_camera_ray(window, *camera);
    float step = 0.1f;

    if(window.is_key_down(GLFW_KEY_LEFT_CONTROL)) {
        colorSelected = 0;
    } else {
        colorSelected = colorCache;
    }

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
        solve_rectangle(lineStart, lineEnd);

        /* distance = 0.0f;
        glm::vec3 lineDir = (lineEnd - lineStart);

        while(distance < glm::length(lineDir)) {
            distance += step;
            glm::vec3 point = lineStart + glm::normalize(lineDir) * distance;

            grid.set(point, colorSelected);
        } */
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
                        grid.set((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step), colorSelected);
                        lastPlace = glfwGetTime();
                    }
                    update_grid();
                    break;
                }
            } else {
                if(grid.point_intersection((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step))) {
                    if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1) && !window.is_key_down(GLFW_KEY_LEFT_CONTROL)) {
                        grid.set((camOrigin + (-camDirection * camOffset)) + ray.direction * (distance - step), colorSelected);
                        lastPlace = glfwGetTime();
                    }
                    update_grid();
                    break;
                }
            }
        }
    }
}

void Editor::solve_rectangle(glm::vec3 start, glm::vec3 end) {
    switch(rectangle) {
        case RECTANGLE_CUBE: {
            ERROR("Making cube");
            glm::vec3 size = end - start;
            start.x = start.x < end.x ? start.x : end.x;
            start.y = start.y < end.y ? start.y : end.y;
            start.z = start.z < end.z ? start.z : end.z;

            for(uint32_t z = 0; z < std::abs(size.z); z++) {
                for(uint32_t y = 0; y < std::abs(size.y); y++) {
                    for(uint32_t x = 0; x < std::abs(size.x); x++) {
                        grid.set(start + glm::vec3(x, y, z), colorSelected);
                    }
                }
            }
            ERROR("Finished cube");
            break;
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

void Editor::resize_callback(int32_t width, int32_t height) {
    this->window.width = width;
    this->window.height = height;

    glViewport(0, 0, width, height);
    camera->projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
}