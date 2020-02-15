#include "Viewport.h"

#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"
#include "System/GridLib.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include "ImGUI/imgui.h"

void Viewport::init() {
    selectedGrid = 0;

    render.init();

    cameraBuffer = RenderLib::create_buffer_stream(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr);
    camera = (Camera*)RenderLib::map_buffer_range(cameraBuffer, GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2);
    RenderLib::buffer_binding_range(cameraBuffer, 0, 0, sizeof(glm::mat4) * 2);
    camera->projection = glm::perspective(glm::radians(45.0f), 720.0f / (480.0f), 0.1f, 100.0f);

    drawing = false;

    panSpeed = 10.0f;
    rotationSpeed = 100.0f;
    camDirection = glm::normalize(glm::vec3(0.0f, 1.0f, -1.0f));

    camOrigin = glm::vec3(16.0f, 16.0f, 0.0f);
    camOffset = 20.0f;

    rectangle = 0;
    camera->view = glm::lookAt(camOrigin + (-camDirection * camOffset), camOrigin, glm::vec3(0.0f, 0.0f, 1.0f));

    extrudeSelect = new glm::vec3(scene->grids[selectedGrid].cache[scene->grids[selectedGrid].cacheIndex].size * scene->grids[selectedGrid].cache[scene->grids[selectedGrid].cacheIndex].size);
    extrudeIndex = 0;
    drawMode = DRAW_MODE_BRUSH;

    update_lights();
    update_sky_color();
    edit = false;

    update_palette();
    update_grid(scene->grids[0].cache[scene->grids[0].cacheIndex]);

    update_cache();

    renderQuad = RenderLib::create_quad();

    framebuffer = TextureLib::create_framebuffer(window.width, window.height);
    uint32_t colorAttachment = TextureLib::create_texture_2d(GL_TEXTURE_2D, window.width, window.height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA, nullptr);
    TextureLib::framebuffer_attachment(colorAttachment, GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);
    framebuffer.texture = colorAttachment;
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window.width, window.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    glEnable(GL_DEPTH_TEST);
    framebuffer.depth = rboDepth;

    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer not complete!" << std::endl;
        raise(SIGABRT);
    }

    scene->colorSelected = 2;

    ShaderLib::uniform_int32(render.shader, "grid", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, scene->grids[selectedGrid].gridTexture);
}

void Viewport::update() {
}

void Viewport::terminate() {
    
}

void Viewport::draw(Cursor cursor, WindowTileInfo tileInfo) {
    glEnable(GL_DEPTH_TEST);
    solve_mouse();
    solve_input();

    if(ImGui::IsWindowHovered()) {
        float offsetX = tileInfo.x * window.width; float offsetY = tileInfo.y * window.height;
        cursor.cursorX = (2.0f * (((float)cursor.cursorX - offsetX) / tileInfo.width)) / (window.width) - 1.0f;
        cursor.cursorY = 1.0 - ((2.0f * ((((float)cursor.cursorY) - offsetY) / tileInfo.height)) / (window.height));

        solve_camera(cursor);
        solve_voxel_placing(cursor);
    }

    RenderLib::buffer_binding_range(cameraBuffer, 0, 0, sizeof(glm::mat4) * 2);
    ShaderLib::uniform_int32(render.shader, "palette", 1);
    render.draw_scene(framebuffer, scene, glm::vec3(0.0f));

    RenderLib::front_face(GL_CW);
    RenderLib::bind_vertex_array(scene->voxelVAO);
    RenderLib::draw_voxel(scene->boxShader, glm::vec3((float)0, (float)0, (float)0), glm::vec3(scene->grids[selectedGrid].size));
    RenderLib::front_face(GL_CCW);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui::GetWindowDrawList()->AddImage((void*)framebuffer.texture, 
                                        ImVec2(tileInfo.x * window.width, tileInfo.y * window.height + 19), 
                                        ImVec2((tileInfo.x + tileInfo.width) * window.width,
                                        (tileInfo.y + tileInfo.height) * window.height), 
                                        ImVec2(0, 1), ImVec2(1, 0));
    this->tileInfo = tileInfo;
    
}

void Viewport::solve_voxel_placing(Cursor cursor) {
    Ray ray;
    ray.create_camera_ray(cursor, *camera);

    ray.origin = (camOrigin + (-camDirection * camOffset));
    float step = 0.01f;

    // If the user wants to delete stuff
    if(window.is_key_down(GLFW_KEY_LEFT_CONTROL))
        scene->colorSelected = 0;
    else
        scene->colorSelected = scene->colorCache;

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
            solve_rectangle(&scene->grids[selectedGrid].cache[scene->grids[selectedGrid].cacheIndex], shapeStart, shapeEnd);
            update_grid(scene->grids[0].cache[scene->grids[0].cacheIndex]);

            if(edit)
                update_cache();
        } else if(drawing && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
            glm::vec3 shapeEndTemp = ray_cast(ray);
            if(std::floor(shapeEnd[0]) != std::floor(shapeEndTemp[0]) ||
                std::floor(shapeEnd[1]) != std::floor(shapeEndTemp[1]) ||
                std::floor(shapeEnd[2]) != std::floor(shapeEndTemp[2])) {

                memcpy(scene->grids[selectedGrid].cache[scene->grids[selectedGrid].cacheIndex].grid,
                   scene->grids[selectedGrid].cache[(scene->grids[selectedGrid].cacheIndex - 1) % CACHE_SIZE].grid,
                   scene->grids[selectedGrid].size * scene->grids[selectedGrid].size * scene->grids[selectedGrid].size);

                shapeEnd = shapeEndTemp;
                solve_rectangle(&scene->grids[0].cache[scene->grids[0].cacheIndex], shapeStart, shapeEnd);
                update_grid(scene->grids[0].cache[scene->grids[0].cacheIndex]);
            }
        }
    } else {
        if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
            glm::vec3 shapeEndTemp = ray_cast(ray);
            if(std::floor(shapeEnd[0]) != std::floor(shapeEndTemp[0]) ||
                std::floor(shapeEnd[1]) != std::floor(shapeEndTemp[1]) ||
                std::floor(shapeEnd[2]) != std::floor(shapeEndTemp[2])) {
                shapeEnd = shapeEndTemp;
                if(scene->grids[selectedGrid].cache[(scene->grids[selectedGrid].cacheIndex - 1) % CACHE_SIZE].point_intersection(shapeEnd) &&
                    scene->grids[selectedGrid].cache[(scene->grids[selectedGrid].cacheIndex) % CACHE_SIZE].get(shapeEnd) != scene->colorSelected) {
                    scene->grids[selectedGrid].cache[(scene->grids[selectedGrid].cacheIndex) % CACHE_SIZE].set(shapeEnd, scene->colorSelected);
                    
                    edit = true;
                    update_grid(scene->grids[selectedGrid].cache[scene->grids[selectedGrid].cacheIndex]);
                }
            }
        } else {
            if(edit) {
                update_cache();
                edit = false;
            }
        }
    }

    if(window.is_key_down(GLFW_KEY_E)) {
        ray.create_camera_ray(cursor, *camera);
        float distance = 0.0f;
        float step = 0.05f;

        while(distance < 100.0f) {
            distance += step;
            if(scene->grids[selectedGrid].cache[scene->grids[selectedGrid].cacheIndex].get((camOrigin + (-camDirection * camOffset)) + ray.direction * distance) > 0) {
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

                update_grid(scene->grids[0].cache[scene->grids[0].cacheIndex]);
                break;
            }
        }
    }
}

glm::vec3 Viewport::ray_cast(Ray ray) {
    float distance = 0.0f;
    float step = 0.1f;

    Grid<int8_t> previousGrid = scene->grids[selectedGrid].cache[(scene->grids[selectedGrid].cacheIndex - 1) % CACHE_SIZE];
    while(distance < 240.0f) {
        distance += step;
        if(previousGrid.point_intersection(ray.origin + ray.direction * distance)) {
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

void Viewport::flood_fill(glm::vec3 position, glm::vec3 normal) {
    Grid<int8_t>* grid = &scene->grids[selectedGrid].cache[scene->grids[selectedGrid].cacheIndex];

    if(position.x < 0 || position.x > 32 ||
        position.y  < 0 || position.y > 32 ||
        position.z < 0 || position.z > 32) {
        return;
    } if(grid->get(position) <= 0)
        return;
    if(grid->get(position) == scene->colorSelected)
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
        grid->set(position + normal, scene->colorSelected);

        flood_fill(position + right, normal);
        flood_fill(position + forward, normal);
        flood_fill(position - right, normal);
        flood_fill(position - forward, normal);
    }

    return;
}

void Viewport::solve_input() {
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

void Viewport::solve_rectangle(Grid<int8_t>* grid, glm::vec3 start, glm::vec3 end) {
    switch(rectangle) {
        case RECTANGLE_CUBE: {
            if(start.x > end.x) { std::swap(start.x, end.x); }
            if(start.y > end.y) { std::swap(start.y, end.y); }
            if(start.z > end.z) { std::swap(start.z, end.z); }

            for(float x = std::floor(start.x); x <= std::floor(end.x); x++) {
                for(float y = std::floor(start.y); y <= std::floor(end.y); y++) {
                    for(float z = std::floor(start.z); z <= std::floor(end.z); z++) {
                        if(grid->set(glm::vec3(x, y, z), scene->colorSelected))
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

                if(grid->set(start + direction * distance, scene->colorSelected))
                    edit = true;
            }
            break;
        }
    }
}

void Viewport::solve_mouse() {
    glm::vec3 cursor = window.get_normalized_cursor_pos();
    mouseDeltaX = mouseLastX - cursor.x;
    mouseDeltaY = mouseLastY - cursor.y;

    mouseLastX = cursor.x;
    mouseLastY = cursor.y;
}

void Viewport::update_lights() {
    void* pointer = RenderLib::map_buffer_range(scene->lightBuffer, GL_UNIFORM_BUFFER, 0, sizeof(Light) * scene->lightBufferSize);
    memcpy(pointer, scene->lights, sizeof(Light) * MAX_LIGHT_COUNT);

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER); 
}

void Viewport::update_sky_color() {
    void* pointer = RenderLib::map_buffer_range(scene->lightBuffer, GL_UNIFORM_BUFFER, sizeof(Light) * scene->lightBufferSize, sizeof(glm::vec4));
    memcpy(pointer, &scene->skyColor.x, sizeof(glm::vec4));

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER);
}

void Viewport::update_grid(Grid<int8_t> grid) {
    TextureLib::update_texture_3d(scene->grids[0].gridTexture, grid.size, grid.size, grid.size, grid.grid);
}

void Viewport::update_palette() {
    TextureLib::update_texture_1d(scene->paletteTexture, 256, scene->palette);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, scene->paletteTexture);
    ShaderLib::uniform_int32(render.shader, "palette", 1);
}

void Viewport::update_cache() {
    ERROR("New cache " << (scene->grids[selectedGrid].cacheIndex + 1));
    if(scene->grids[selectedGrid].undoCount > 0) {
        scene->grids[selectedGrid].undoCount = 0;
        scene->grids[selectedGrid].usedCache = 0;
    }

    uint32_t original = scene->grids[selectedGrid].cacheIndex;

    scene->grids[selectedGrid].cacheIndex++;
    scene->grids[selectedGrid].cacheIndex %= CACHE_SIZE;
    memcpy(scene->grids[selectedGrid].cache[scene->grids[selectedGrid].cacheIndex].grid, scene->grids[selectedGrid].cache[original].grid, std::pow(/* scene->grids[selectedGrid].quadMesh.width */128, 3));

    if(scene->grids[selectedGrid].usedCache < CACHE_SIZE)
        scene->grids[selectedGrid].usedCache++;

    edit = false;
}

void Viewport::undo() {
    if(scene->grids[selectedGrid].undoCount < scene->grids[selectedGrid].usedCache) { 
        scene->grids[selectedGrid].undoCount++;
        scene->grids[selectedGrid].cacheIndex--;
        scene->grids[selectedGrid].cacheIndex %= CACHE_SIZE;
        MESSAGE("Undo " << scene->grids[selectedGrid].cacheIndex);
        update_grid(scene->grids[0].cache[scene->grids[0].cacheIndex]);
    }
}

void Viewport::redo() {
    if(scene->grids[selectedGrid].undoCount > 0) {
        scene->grids[selectedGrid].undoCount--;
        scene->grids[selectedGrid].cacheIndex++;
        scene->grids[selectedGrid].cacheIndex %= CACHE_SIZE;
        MESSAGE("Undo " << scene->grids[selectedGrid].cacheIndex);
        update_grid(scene->grids[0].cache[scene->grids[0].cacheIndex]);
    }
}

void Viewport::solve_camera(Cursor cursor) {
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

void Viewport::resize_callback(uint32_t width, uint32_t height) {
    this->window.width = width;
    this->window.height = height;

    camera->projection = glm::perspective(glm::radians(45.0f), (float)(width * tileInfo.width) / (float)(height * tileInfo.height), 0.1f, 1000.0f);
    framebuffer.width = width;
    framebuffer.height = height;

    glDeleteTextures(1, &framebuffer.texture);
    glDeleteRenderbuffers(1, &framebuffer.depth);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.framebuffer);

    uint32_t colorAttachment = TextureLib::create_texture_2d(GL_TEXTURE_2D, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA, nullptr);
    TextureLib::framebuffer_attachment(colorAttachment, GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);
    framebuffer.texture = colorAttachment;

    glGenRenderbuffers(1, &framebuffer.depth);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depth);
}