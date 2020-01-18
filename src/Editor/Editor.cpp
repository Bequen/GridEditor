#include "Editor.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Rendering/RenderLib.h"
#include <avg/Debug.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Rendering/TextureLib.h"
#include "Rendering/ShaderLib.h"
#include <csignal>

void Editor::init() {
    MESSAGE("Starting the editor initialization");

    selectedGrid = 0;
    scene = Scene(1);

    scene.init();
    render.init(nullptr);

    uint32_t cameraBuffer = RenderLib::create_buffer_stream(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr);
    camera = (Camera*)RenderLib::map_buffer_range(cameraBuffer, GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2);
    RenderLib::buffer_binding_range(cameraBuffer, 0, 0, sizeof(glm::mat4) * 2);
    camera->projection = glm::perspective(glm::radians(45.0f), 720.0f / 480.0f, 0.1f, 1000.0f);

    colorSelected = 0;
    drawing = false;

    palette = (RGB32*)malloc(sizeof(RGB32) * 256);
    memset(palette, 0, sizeof(RGB32) * 256);
    paletteTexture = TextureLib::create_texture_1d(256, GL_RGB, GL_RGB, palette);

    panSpeed = 10.0f;
    rotationSpeed = 100.0f;
    camDirection = glm::normalize(glm::vec3(0.0f, 1.0f, -1.0f));

    camOrigin = glm::vec3(16.0f, 16.0f, 0.0f);
    camOffset = 20.0f;

    rectangle = 0;
    camera->view = glm::lookAt(camOrigin + (-camDirection * camOffset), camOrigin, glm::vec3(0.0f, 0.0f, 1.0f));

    extrudeSelect = new glm::vec3(scene.grids[selectedGrid].cache[scene.grids[selectedGrid].cacheIndex].size * scene.grids[selectedGrid].cache[scene.grids[selectedGrid].cacheIndex].size);
    extrudeIndex = 0;
    drawMode = DRAW_MODE_BRUSH;

    update_lights();
    update_sky_color();
    edit = false;

    update_palette();
    update_grid();
}

void Editor::update() {
    solve_mouse();
    solve_input();

    if(!ImGui::IsAnyWindowHovered() && !ImGui::IsAnyItemHovered()) {
        solve_camera();
        solve_voxel_placing();
    }

    render.draw_scene(scene);
    draw_ui();
}

void Editor::draw_ui() {
    draw_palette();
    draw_toolbar();
    draw_scene_setup();
}

void Editor::draw_palette() {
    ImGui::Begin("Pallette", nullptr);

    ImGui::BeginChild("PickerWindow", ImVec2(200.0f, 200.0f), true);
    // Stupid workaround
    if(colorSelected > 255)
        colorSelected = 1;
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
                ImGui::SameLine((i % rowSize) * (buttonSize + padding) + padding * 2);
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

    if (ImGui::BeginCombo("##combo", items[rectangle]))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            bool is_selected = (rectangle == n);
            if (ImGui::Selectable(items[n], is_selected))
                rectangle = n;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::End();
}

void Editor::draw_scene_setup() {
    ImGui::Begin("Scene Setup");

    ImGui::BeginChild("Lighting", ImVec2(200.0f, 200.0f), true);
    if(ImGui::ColorPicker4("Sky Color", &scene.skyColor.r)) {
        render.skyColor = scene.skyColor;
        update_sky_color();
    }
    ImGui::EndChild();

    ImGui::BeginChild("Scene Lights");
    for(uint32_t i = 0; i < scene.lightCount; i++) {
        char* buffer = new char[256];
        sprintf(buffer, "LightLabel%i", i);

        ImGui::BeginChild(buffer, ImVec2(0.0f, 100.0f), true);
        ImGui::Text("Light %i", i);
        if(ImGui::InputFloat3("Position", &scene.lights[i].position.x, 3)) {
            update_lights();
            ERROR("Updating Lights");
        } if(ImGui::InputFloat3("Direction", &scene.lights[i].direction.x, 3)) {
            update_lights();
            ERROR("Updating Lights");
        }
        ImGui::EndChild();

        delete [] buffer;
    }

    {
        ImGui::BeginChild("AddLightLabel", ImVec2(200.0f, 0.0f));
        if(ImGui::Button("AddLightButton")) {
            scene.lightCount++;
        }
        ImGui::EndChild();
    }

    ImGui::EndChild();

    ImGui::End();
}

void Editor::terminate() {

}

void Editor::solve_voxel_placing() {
    Ray ray;
    ray.create_camera_ray(window, *camera);
    ray.origin = (camOrigin + (-camDirection * camOffset));
    float step = 0.01f;

    // If the user wants to delete stuff
    if(window.is_key_down(GLFW_KEY_LEFT_CONTROL))
        colorSelected = 0;
    else
        colorSelected = colorCache;

    if(window.is_key_down(GLFW_KEY_LEFT_SHIFT))
        drawMode = DRAW_MODE_SHAPE;
    else
        drawMode = DRAW_MODE_BRUSH;

    if(drawMode == DRAW_MODE_SHAPE) {
        if(!drawing && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
            drawing = true;

            shapeStart = ray_cast(ray);
        } else if(drawing && !window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
            drawing = false;

            shapeEnd = ray_cast(ray);

            solve_rectangle(shapeStart, shapeEnd);
            update_grid();

            if(edit)
                update_cache();
        }
    } else {
        if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
            if(scene.grids[selectedGrid].cache[scene.grids[selectedGrid].cacheIndex].point_intersection(ray_cast(ray))) {
                scene.grids[selectedGrid].cache[scene.grids[selectedGrid].cacheIndex].set(ray_cast(ray), colorSelected);
                edit = true;
            }
            update_grid();
        } else {
            if(edit)
                update_cache();
        }
    }

    if(window.is_key_down(GLFW_KEY_E)) {
        ray.create_camera_ray(window, *camera);
        float distance = 0.0f;
        float step = 0.01f;
        ERROR("FLOOD FILL");

        while(distance < 100.0f) {
            distance += step;
            if(scene.grids[selectedGrid].cache[scene.grids[selectedGrid].cacheIndex].get((camOrigin + (-camDirection * camOffset)) + ray.direction * distance) > 0) {
                glm::vec3 position = (camOrigin + (-camDirection * camOffset)) + ray.direction * distance;
                position.x = std::floor(position.x);
                position.y = std::floor(position.y);
                position.z = std::floor(position.z);
                glm::vec3 normal = (camOrigin + (-camDirection * camOffset)) + (ray.direction * (distance - step));
                normal.x = std::floor(normal.x);
                normal.y = std::floor(normal.y);
                normal.z = std::floor(normal.z);

                normal = normal - position;

                ERROR("Flood fill on " << position.x << "|" << position.y << "|" << position.z << "\n" <<
                        normal.x << "|" << normal.y << "|" << normal.z);

                flood_fill(position, normal);

                update_grid();
                break;
            }
        }
    }
}

glm::vec3 Editor::ray_cast(Ray ray) {
    float distance = 0.0f;
    float step = 0.1f;

    Grid<int8_t> previousGrid = scene.grids[selectedGrid].cache[(scene.grids[selectedGrid].cacheIndex - 1) % CACHE_SIZE];
    while(distance < 100.0f) {
        distance += step;
        if(scene.grids[selectedGrid].cache[(scene.grids[selectedGrid].cacheIndex - 1) % CACHE_SIZE].point_intersection(ray.origin + ray.direction * distance)) {
            if(previousGrid.get(ray.origin + ray.direction * distance) > 0) {
                if(window.is_key_down(GLFW_KEY_LEFT_CONTROL))
                    return ray.origin + ray.direction * (distance);
                else
                    return ray.origin + ray.direction * (distance - step);
            }
        } else {
            if(previousGrid.point_intersection(ray.origin + ray.direction * (distance - step))) {
                return ray.origin + ray.direction * (distance - step);
            }
        }
    }
}

void Editor::flood_fill(glm::vec3 position, glm::vec3 normal) {
    Grid<int8_t>* grid = &scene.grids[selectedGrid].cache[scene.grids[selectedGrid].cacheIndex];

    if(position.x < 0 || position.x > 32 ||
        position.y  < 0 || position.y > 32 ||
        position.z < 0 || position.z > 32) {
        return;
    } if(grid->get(position) <= 0)
        return;
    if(grid->get(position) == colorSelected)
        return;

    glm::vec3 right = glm::vec3((1.0f - std::abs(normal.x)), (1.0f - std::abs(normal.y)), (1.0f - std::abs(normal.z)));
    for(uint32_t i = 0; i < 3; i++) {
        if(right[i] == 1.0f) {
            right[i] = 0.0f;
            break;
        }
    }
    
    glm::vec3 forward = glm::cross(right, normal);
    if(grid->get(position + normal) <= 0) {
        grid->set(position, colorSelected);

        flood_fill(position + right, normal);
        flood_fill(position + forward, normal);
        flood_fill(position - right, normal);
        flood_fill(position - forward, normal);
    }

    return;
}

void Editor::solve_input() {
    if(!polygonMode && window.is_key_down(GLFW_KEY_Z) && !window.is_key_down(GLFW_KEY_LEFT_SHIFT) && !window.is_key_down(GLFW_KEY_LEFT_CONTROL)) {
        render.polygonMode = 1 - render.polygonMode;
        polygonMode = true;
    } else {
        if(!window.is_key_down(GLFW_KEY_Z)) {
            polygonMode = false;
        }
        if(window.is_key_down(GLFW_KEY_LEFT_CONTROL) && window.is_key_down(GLFW_KEY_Z) && !window.is_key_down(GLFW_KEY_LEFT_SHIFT)) {
            if(undoState == STATE_NONE) {
                undoState = STATE_PRESS;
                undo();
            }
        } else {
            undoState = STATE_NONE;
        }

        if(window.is_key_down(GLFW_KEY_LEFT_CONTROL) && window.is_key_down(GLFW_KEY_LEFT_SHIFT) && window.is_key_down(GLFW_KEY_Z)) {
            if(redoState == STATE_NONE) {
                redoState = STATE_PRESS;
                redo();
            }
        } else {
            redoState = STATE_NONE;
        }
    }
}

void Editor::solve_rectangle(glm::vec3 start, glm::vec3 end) {
    Grid<int8_t>* grid = &scene.grids[selectedGrid].cache[scene.grids[selectedGrid].cacheIndex];
    switch(rectangle) {
        case RECTANGLE_CUBE: {
            if(start.x > end.x) { std::swap(start.x, end.x); }
            if(start.y > end.y) { std::swap(start.y, end.y); }
            if(start.z > end.z) { std::swap(start.z, end.z); }

            for(float x = std::floor(start.x); x <= std::floor(end.x); x++) {
                for(float y = std::floor(start.y); y <= std::floor(end.y); y++) {
                    for(float z = std::floor(start.z); z <= std::floor(end.z); z++) {
                        if(grid->set(glm::vec3(x, y, z), colorSelected))
                            edit = true;
                    }
                }
            }
            break;
        } case RECTANGLE_LINE: {
            glm::vec3 direction = end - start;
            float length = glm::length(direction);
            direction = glm::normalize(direction);

            float distance = 0.0f;
            float step = 0.1f;

            while(distance < length) {
                distance += step;

                if(grid->set(start + direction * distance, colorSelected))
                    edit = true;
            }
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

void Editor::update_lights() {
    /* void* pointer = RenderLib::map_buffer_range(lightBuffer.lightBuffer, GL_UNIFORM_BUFFER, 0, sizeof(Light) * MAX_LIGHT_COUNT);
    memcpy(pointer, lightBuffer.lights, sizeof(Light) * MAX_LIGHT_COUNT);

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER); */

    void* pointer = RenderLib::map_buffer_range(scene.lightBuffer, GL_UNIFORM_BUFFER, 0, sizeof(Light) * scene.lightBufferSize);
    memcpy(pointer, scene.lights, sizeof(Light) * MAX_LIGHT_COUNT);

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER); 
}

void Editor::update_sky_color() {
    void* pointer = RenderLib::map_buffer_range(scene.lightBuffer, GL_UNIFORM_BUFFER, sizeof(Light) * scene.lightBufferSize, sizeof(glm::vec4));
    memcpy(pointer, &scene.skyColor.x, sizeof(glm::vec4));

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER);
}

void Editor::update_grid() {
    TextureLib::update_texture_3d(scene.grids[0].gridTexture, 32, 32, 32, scene.grids[0].cache[scene.grids[0].cacheIndex].grid);
    /* glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, scene.grids[0].gridTexture);
    ShaderLib::uniform_int32(render.shader, "grid", 0); */

}

void Editor::update_palette() {
    TextureLib::update_texture_1d(paletteTexture, 256, palette);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, paletteTexture);
    ShaderLib::uniform_int32(render.shader, "palette", 1);
}

void Editor::update_cache() {
    ERROR("New cache");
    if(scene.grids[selectedGrid].undoCount > 0) {
        ERROR("Resetting cache");
        scene.grids[selectedGrid].undoCount = 0;
        scene.grids[selectedGrid].usedCache = 0;
    }

    uint32_t original = scene.grids[selectedGrid].cacheIndex;

    scene.grids[selectedGrid].cacheIndex++;
    scene.grids[selectedGrid].cacheIndex %= CACHE_SIZE;
    memcpy(scene.grids[selectedGrid].cache[scene.grids[selectedGrid].cacheIndex].grid, scene.grids[selectedGrid].cache[original].grid, std::pow(32, 3));
    ERROR("New index " << scene.grids[selectedGrid].cacheIndex)
    if(scene.grids[selectedGrid].usedCache < CACHE_SIZE)
        scene.grids[selectedGrid].usedCache++;

    edit = false;
}

void Editor::undo() {
    /* if(undoCount < usedCache) {
        undoCount++;
        cacheIndex--;
        cacheIndex %= cacheSize;
        render.grid = &cache[cacheIndex];
    } */
}

void Editor::redo() {
    /* if(undoCount > 0) {
        undoCount--;
        cacheIndex++;
        cacheIndex %= cacheSize;
        render.grid = &cache[cacheIndex];
    } */
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
        camDirection = glm::rotate(camDirection, (float)mouseDeltaX * rotationSpeed * (float)*deltaTime, glm::vec3(0.0f, 0.0f, 1.0f));
        camDirection = glm::rotate(camDirection, (float)mouseDeltaY * rotationSpeed * (float)*deltaTime, -glm::normalize(glm::vec3(camera->view[0][0], camera->view[1][0], camera->view[2][0])));
    }

    camera->view = glm::lookAt(camOrigin + (-camDirection * camOffset), camOrigin, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Editor::resize_callback(int32_t width, int32_t height) {
    this->window.width = width;
    this->window.height = height;

    glViewport(0, 0, width, height);
    camera->projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
}