#include "Viewport.h"

#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"
#include "System/Voxels/GridLib.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/intersect.hpp>
#include "ImGUI/imgui.h"
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;


///
/// Initialization
///
void Viewport::init() {
    // Initializes few things
    render.init();
    camera.init();

    // Set some default values
    selectedGrid = 0;
    rectangle = 0;
    extrudeIndex = 0;
    isEditMode = 1;
    drawing = false;
    extruding = false;
    edit = false;

    extrudeSelect = new glm::vec3(0.0f);
    drawMode = DRAW_MODE_BRUSH;

    if(scene->gridsCount > 0)
        change_grid(0);
    else
        change_grid(-1);

    // Updates all that is needed
    update_grid(scene->_grids[selectedGrid]);
    update_cache();

    init_framebuffer();
    init_profiler();

    ShaderLib::uniform_int32(render.voxelProgram, "palette", 1);
    scene->colorSelected = 2;
    floodFillSelection.floodFillCount = 0;

    input.init(32);
    input.add_key({GLFW_KEY_TAB, 0});
    input.add_key({GLFW_KEY_A, 0});
    input.add_key({GLFW_KEY_LEFT_SHIFT, 0});
    input.add_key({GLFW_KEY_LEFT_CONTROL, 0});
    input.add_key({GLFW_KEY_LEFT_ALT, 0});
    input.add_key({GLFW_KEY_1, 0});
    input.add_key({GLFW_KEY_3, 0});
    input.add_key({GLFW_KEY_5, 0});
    input.add_key({GLFW_KEY_7, 0});
    input.add_key({GLFW_KEY_9, 0});

    input.add_mouse_key({GLFW_MOUSE_BUTTON_1, 0});
    input.add_mouse_key({GLFW_MOUSE_BUTTON_2, 0});
    input.add_mouse_key({GLFW_MOUSE_BUTTON_3, 0});
}

void Viewport::init_framebuffer() {
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
    framebuffer.depth = rboDepth;

    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    assert_msg(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");

    ShaderLib::uniform_int32(render.voxelProgram, "grid", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, scene->_grids[selectedGrid].gridTexture);
}

void Viewport::init_profiler() {
    PerformanceStat _stat;
    _stat.bufferSize = 32;
    _stat.time = new float[32];
    _stat.id = 0;
    _stat.index = 0;
    _stat.name = "Rendering";
    memset(_stat.time, 0, sizeof(float) * 32);

    profiler.stats[0] = _stat;
    profiler.count++;
    stat = &profiler.stats[0];
}



///
/// Updates
/// 
void Viewport::update() {
}

void Viewport::terminate() {
    
}

void Viewport::draw(Cursor cursor, WindowTileInfo tileInfo) {
    input.update(window);
    solve_input();

    // Capture mouse for camera, but only if tile is hovered
    if(ImGui::IsWindowHovered()) {
        camera.update(input);
        float offsetX = tileInfo.x * window.width; float offsetY = tileInfo.y * window.height;
        cursor.cursorX = (2.0f * (((float)cursor.cursorX - offsetX) / tileInfo.width)) / (window.width) - 1.0f;
        cursor.cursorY = 1.0 - ((2.0f * ((((float)cursor.cursorY) - offsetY) / tileInfo.height)) / (window.height));

        if(selectedGrid != -1) {
            if(input.get(GLFW_KEY_TAB) == KEY_STATE_PRESS) {
                MESSAGE("Leaving edit mode");
                change_grid(-1);
            }

            solve_voxel_placing(cursor);
        } else {
            Ray ray = camera.create_ray(window.get_normalized_cursor_pos());
            //ray.origin = (camOrigin + (-camDirection * camOffset));

            if(input.get(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                for(uint32_t i = 0; i < scene->gridsCount; i++) {
                    float result = 0.0f;
                    glm::intersectRaySphere(ray.origin, ray.direction, glm::vec3(scene->_grids[i].width, scene->_grids[i].depth, scene->_grids[i].height) / 2.0f, 16.0f * 16.0f, result);

                    if(result != 0.0f) {
                        MESSAGE(scene->names[i]);
                        change_grid(i);
                    }
                }
            } else if(input.get(GLFW_KEY_LEFT_SHIFT) != KEY_STATE_NONE && input.get(GLFW_KEY_A) == KEY_STATE_PRESS) {
                MESSAGE("Opening object creation context menu");
                
            }
        }
    }

    // Bounds the camera
    RenderLib::buffer_binding_range(camera.cameraBuffer, 0, 0, sizeof(glm::mat4) * 2);

    // Actual rendering code
    auto start_time = Clock::now();

    render.draw_scene(framebuffer, scene);
    //render.draw_sky();
    // Draws the cage
    if(selectedGrid != -1) {
        render.draw_grid(tempGrid, scene->transforms[selectedGrid]);

        RenderLib::front_face(GL_CW);
        RenderLib::bind_vertex_array(scene->voxelVAO);
        RenderLib::draw_voxel(scene->boxShader, glm::vec3((float)0, (float)0, (float)0), glm::vec3(scene->_grids[selectedGrid].width, scene->_grids[selectedGrid].depth, scene->_grids[selectedGrid].height));
        RenderLib::front_face(GL_CCW);
    } else {
        for(uint32_t i = 0; i < scene->gridsCount; i++) {
            RenderLib::bind_vertex_array(scene->voxelVAO);
            
            render.draw_grid(tempGrid, scene->transforms[i]);
            RenderLib::draw_voxel(scene->boxShader, glm::vec3((float)0, (float)0, (float)0), glm::vec3(scene->_grids[i].width, scene->_grids[i].depth, scene->_grids[i].height));
        }
    }

    auto end_time = Clock::now();
    stat->count++;
    stat->sum += (float)std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Draws resulting image into the ImGui window
    ImGui::GetWindowDrawList()->AddImage((void*)framebuffer.texture, 
                                        ImVec2(tileInfo.x * window.width, tileInfo.y * window.height + 19), 
                                        ImVec2((tileInfo.x + tileInfo.width) * window.width,
                                        (tileInfo.y + tileInfo.height) * window.height), 
                                        ImVec2(0, 1), ImVec2(1, 0));
    this->tileInfo = tileInfo;
}

void Viewport::solve_voxel_placing(Cursor cursor) {
    // Initialize a new ray
    Ray ray = camera.create_ray(glm::vec3(cursor.cursorX, cursor.cursorY, 0.0f));
    //ray.create_camera_ray(cursor, *camera, cameraMode);
    //ray.origin = (camOrigin + (-camDirection * camOffset));
    const float step = 0.01f; // Ray step

    // If the user wants to delete stuff
    if(window.is_key_down(GLFW_KEY_LEFT_CONTROL))
        scene->colorSelected = 0;
    else
        scene->colorSelected = scene->colorCache;



    // If user want to draw a shape
    if(window.is_key_down(GLFW_KEY_LEFT_SHIFT))
        drawMode = DRAW_MODE_SHAPE;
    else
        drawMode = DRAW_MODE_BRUSH;


    // If the shape is being drawn
    if(!window.is_key_down(GLFW_KEY_LEFT_ALT) && drawMode == DRAW_MODE_SHAPE) {
        if(!drawing && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
            drawing = true;
            shapeStart = ray_cast(ray);
        } else if(drawing && !window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
            drawing = false;

            shapeEnd = ray_cast(ray);
            memcpy(scene->_grids[selectedGrid].buffer, tempGrid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
            solve_rectangle(&scene->_grids[selectedGrid], shapeStart, shapeEnd);
            update_grid(scene->_grids[selectedGrid]);

            if(edit)
                update_cache();
        } else if(drawing && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
            glm::vec3 shapeEndTemp = ray_cast(ray);
            if(std::floor(shapeEnd[0]) != std::floor(shapeEndTemp[0]) ||
                std::floor(shapeEnd[1]) != std::floor(shapeEndTemp[1]) ||
                std::floor(shapeEnd[2]) != std::floor(shapeEndTemp[2])) {

                memcpy(tempGrid.buffer, scene->_grids[selectedGrid].buffer, tempGrid.width * tempGrid.depth * tempGrid.height);

                shapeEnd = shapeEndTemp;
                solve_rectangle(&tempGrid, shapeStart, shapeEnd);
                update_grid(tempGrid);
            }
        }
    } else {
        if(!window.is_key_down(GLFW_KEY_LEFT_ALT) && !drawing && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
            glm::vec3 shapeEndTemp = ray_cast(ray);
            if(std::floor(shapeEnd[0]) != std::floor(shapeEndTemp[0]) ||
                std::floor(shapeEnd[1]) != std::floor(shapeEndTemp[1]) ||
                std::floor(shapeEnd[2]) != std::floor(shapeEndTemp[2])) {

                shapeEnd = shapeEndTemp;

                if(scene->_grids[selectedGrid].intersects(shapeEnd)) {
                    tempGrid.set(shapeEnd, scene->colorSelected);
                    update_grid(tempGrid);
                    edit = true;
                }
            }
        } else {
            if(!window.is_key_down(GLFW_KEY_LEFT_ALT) && edit) {
                update_cache();
                memcpy(scene->_grids[selectedGrid].buffer, tempGrid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
                update_grid(scene->_grids[selectedGrid]);
                edit = false;
            }
        }
    }

    if(window.is_key_down(GLFW_KEY_LEFT_ALT) && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1) && !extruding) {
        float distance = 0.0f;
        float step = 0.05f;


        while(distance < 100.0f) {
            distance += step;
            if(scene->_grids[selectedGrid].get((camOrigin + (-camDirection * camOffset)) + ray.direction * distance) > 0) {
                glm::vec3 position = (camOrigin + (-camDirection * camOffset)) + ray.direction * distance;
                position.x = std::floor(position.x);
                position.y = std::floor(position.y);
                position.z = std::floor(position.z);

                glm::vec3 normal = (camOrigin + (-camDirection * camOffset)) + (ray.direction * (distance - step));
                normal.x = std::floor(normal.x);
                normal.y = std::floor(normal.y);
                normal.z = std::floor(normal.z);

                if(floodFillSelection.selected)
                    delete [] floodFillSelection.selected;
                floodFillSelection.selected = new uint32_t[tempGrid.width * tempGrid.depth * tempGrid.height];
                memset(floodFillSelection.selected, 0, sizeof(uint32_t) * (tempGrid.width * tempGrid.depth));
                
                normal = normal - position;
                normal = glm::normalize(normal);
                flood_fill(position, normal, scene->colorSelected);
                shapeStart = position;
                extruding = true;
                ERROR("Flood fill " << shapeStart.z);
                shapeEnd = shapeStart;
                floodFillSelection.normal = normal;
                glm::vec3 n = floodFillSelection.normal * glm::vec3(1.0, tempGrid.width, tempGrid.width * tempGrid.depth);
                floodFillSelection.normalOffset = n.x + n.y + n.z;

                update_grid(tempGrid);
                break;
            }
        }
    } else if(window.is_key_down(GLFW_KEY_LEFT_ALT) && window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1) && extruding) {
        //ray.create_camera_ray(cursor, *camera, cameraMode);
        float distance = 0.0f;
        float step = 0.05f;

        glm::vec3 c = (camOrigin + (-camDirection * camOffset));

        float result = 0.0f;
        glm::vec3 n = c - shapeStart;
        n *= glm::vec3(1.0, 1.0, 1.0) - floodFillSelection.normal;
        n = glm::normalize(n);
        glm::vec3 s = shapeStart;
        s *= glm::vec3(1.0f, 1.0f, 1.0f) - floodFillSelection.normal;
        //s.z = 0.0f;
        glm::intersectRayPlane(c, ray.direction, s, n, result);
        glm::vec3 r = (c + ray.direction * result);
        r *= floodFillSelection.normal;
        result = r.x + r.y + r.z;
        float offset = (shapeStart * floodFillSelection.normal).x +(shapeStart * floodFillSelection.normal).y + (shapeStart * floodFillSelection.normal).z;

        shapeEnd = shapeStart + floodFillSelection.normal * result;

        memcpy(tempGrid.buffer, scene->_grids[selectedGrid].buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
        
        ERROR(result << ":" << shapeStart.z);
        MESSAGE(floodFillSelection.normal.x << "|" << floodFillSelection.normal.y << "|" << floodFillSelection.normal.z);
        extrude(result - offset);
        update_grid(tempGrid);
    } else if((!window.is_key_down(GLFW_KEY_LEFT_ALT) || !window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) && extruding) {
        extruding = false;
        memcpy(scene->_grids[selectedGrid].buffer, tempGrid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
        update_grid(scene->_grids[selectedGrid]);
        floodFillSelection.floodFillCount = 0;
    }
}

void Viewport::extrude(int32_t height) {
    int32_t direction = height > 0 ? 1 : -1;
    for(uint32_t i = 0; i < floodFillSelection.floodFillCount; i++) {
        for(int32_t x = 0; x < height; x++) {
            tempGrid.set(floodFillSelection.selected[i] + x * floodFillSelection.normalOffset, scene->colorSelected);
        }
    }
}

glm::vec3 Viewport::ray_cast(Ray ray) {
    float distance = 0.0f;
    float step = 0.1f;

    _Grid grid = scene->_grids[selectedGrid];
    while(distance < 240.0f) {
        distance += step;
        if(grid.intersects(ray.origin + ray.direction * distance)) {
            if(grid.get(ray.origin + ray.direction * distance) > 0) {
                if(window.is_key_down(GLFW_KEY_LEFT_CONTROL))
                    return ray.origin + ray.direction * (distance);
                else
                    return ray.origin + ray.direction * (distance - step);
            }
        } else {
            if(grid.intersects(ray.origin + ray.direction * (distance - step))) {
                return ray.origin + ray.direction * (distance - step);
            }
        }
    }
}

void Viewport::flood_fill(glm::vec3 position, glm::vec3 normal, int8_t brush) {
    _Grid* grid = &scene->_grids[selectedGrid];

    if(position.x < 0 || position.x > tempGrid.width ||
        position.y  < 0 || position.y > tempGrid.depth ||
        position.z < 0 || position.z > tempGrid.height) {
        return;
    } if(grid->get(position) <= 0)
        return;
    if(grid->get(position) != scene->colorSelected)
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
        //grid->set(position + normal, scene->colorSelected);
        uint32_t index = (position + normal).x + (position + normal).y * (tempGrid.width) + (position + normal).z * (tempGrid.width * tempGrid.depth);
        for(uint32_t i = 0; i < floodFillSelection.floodFillCount; i++)
            if(floodFillSelection.selected[i] == index)
                return;
        //ERROR("Selected index " << index);
        floodFillSelection.selected[floodFillSelection.floodFillCount++] = index;

        flood_fill(position + right, normal, brush);
        flood_fill(position + forward, normal, brush);
        flood_fill(position - right, normal, brush);
        flood_fill(position - forward, normal, brush);
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

    if(input.get(GLFW_KEY_5) == KEY_STATE_PRESS) {
        MESSAGE("Changing mode");
        camera.set_mode(1 - camera.mode);
    } if(input.get(GLFW_KEY_1) == KEY_STATE_PRESS) {
        MESSAGE("Front View");
        camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
        camera.direction = glm::vec3(0.0f, 1.0f, 0.0f);
    } if(input.get(GLFW_KEY_3) == KEY_STATE_PRESS) {
        MESSAGE("Side View");
        camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
        camera.direction = glm::vec3(-1.0f, 0.0f, 0.0f);
    } if(input.get(GLFW_KEY_7) == KEY_STATE_PRESS) {
        MESSAGE("Top View");
        camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
        camera.direction = glm::vec3(0.0f, 0.0f, -0.9f);
    } if(input.get(GLFW_KEY_9) == KEY_STATE_PRESS) {
        MESSAGE("Opposite View");
        camera.direction *= glm::vec3(-1.0f);
    }
}

void Viewport::solve_rectangle(_Grid* grid, glm::vec3 start, glm::vec3 end) {
    switch(rectangle) {
        case RECTANGLE_CUBE: {
            if(start.x > end.x) { std::swap(start.x, end.x); }
            if(start.y > end.y) { std::swap(start.y, end.y); }
            if(start.z > end.z) { std::swap(start.z, end.z); }

            uint32_t index = 0;
            for(float x = std::floor(start.x); x <= std::floor(end.x); x++) {
                for(float y = std::floor(start.y); y <= std::floor(end.y); y++) {
                    for(float z = std::floor(start.z); z <= std::floor(end.z); z++) {
                        grid->set(glm::vec3(x, y, z), scene->colorSelected);
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

                grid->set(start + direction * distance, scene->colorSelected);
                edit = true;
            }
            break;
        }
    }
}

void Viewport::update_grid(_Grid grid) {
    TextureLib::update_texture_3d(grid.gridTexture, grid.width, grid.depth, grid.height, grid.buffer);
}

void Viewport::update_cache() {
    /* if(scene->grids[selectedGrid].undoCount > 0) {
        scene->grids[selectedGrid].undoCount = 0;
        scene->grids[selectedGrid].usedCache = 0;
    }

    uint32_t original = scene->grids[selectedGrid].cacheIndex;

    scene->grids[selectedGrid].cacheIndex++;
    scene->grids[selectedGrid].cacheIndex %= CACHE_SIZE;
    memcpy(scene->grids[selectedGrid].cache[scene->grids[selectedGrid].cacheIndex].grid, scene->grids[selectedGrid].cache[original].grid, std::pow(128, 3));

    if(scene->grids[selectedGrid].usedCache < CACHE_SIZE)
        scene->grids[selectedGrid].usedCache++;

    edit = false; */
}

void Viewport::undo() {
    /* if(scene->grids[selectedGrid].undoCount < scene->grids[selectedGrid].usedCache) { 
        scene->grids[selectedGrid].undoCount++;
        scene->grids[selectedGrid].cacheIndex--;
        scene->grids[selectedGrid].cacheIndex %= CACHE_SIZE;
        MESSAGE("Undo " << scene->grids[selectedGrid].cacheIndex);
        update_grid(scene->grids[0].cache[scene->grids[0].cacheIndex]);
    } */
}

void Viewport::redo() {
    /* if(scene->grids[selectedGrid].undoCount > 0) {
        scene->grids[selectedGrid].undoCount--;
        scene->grids[selectedGrid].cacheIndex++;
        scene->grids[selectedGrid].cacheIndex %= CACHE_SIZE;
        MESSAGE("Undo " << scene->grids[selectedGrid].cacheIndex);
        update_grid(scene->grids[0].cache[scene->grids[0].cacheIndex]);
    } */
}


void Viewport::change_grid(int32_t index) {
    if(index > scene->gridsCount)
        return;

    selectedGrid = index;
    MESSAGE("Changing grid to " << selectedGrid);
    // If the index says to exit edit mode
    if(index == -1)
        return;

    // If there is something in cache
    // TODO might be cool to save it for future undo/redo :)
    if(tempGrid.buffer)
        delete [] tempGrid.buffer;

    tempGrid = scene->_grids[selectedGrid];

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, tempGrid.gridTexture);

    ERROR("GRID " << tempGrid.width << "|" << tempGrid.height << "|" << tempGrid.depth);
    tempGrid.buffer = new int8_t[tempGrid.width * tempGrid.depth * tempGrid.height];
    memcpy(tempGrid.buffer, scene->_grids[selectedGrid].buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
}


void Viewport::resize_callback(uint32_t width, uint32_t height) {
    this->window.width = width;
    this->window.height = height;

    if(tileInfo.width == 0.0)
        tileInfo.width = 1.0f;
    if(tileInfo.height == 0.0f)
        tileInfo.height = 1.0f;
    //camera->projection = glm::perspective(glm::radians(45.0f), (float)(width * tileInfo.width) / (float)(height * tileInfo.height), 0.1f, 1000.0f);
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