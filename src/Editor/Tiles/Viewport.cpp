#include "Viewport.h"

#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/intersect.hpp>
#include "ImGui/imgui.h"
#include <chrono>
#include <avg/Debug.h>
#include "Rendering/TextureLib.h"
#include "Editor/Viewport/SpriteEditor.h"
typedef std::chrono::high_resolution_clock Clock;


///
/// Initialization
///
void Viewport::init() {
    // Initializes few things
    info.camera.init();

    // Set some default values
    snapping = false;
    snappingSwitch = 0;
    isDrawing = false;
    drawMode = DRAW_MODE_BRUSH;
    brushMode = BRUSH_MODE_ADD;
    brushModeCache = BRUSH_MODE_ADD;
    shapeMode = SHAPE_CUBE;
    transformMode = TRANSFORM_MODE_NONE;
    tempGrid.buffer = nullptr;

    if(scene->selected != nullptr) {
        select_grid((Grid*)scene->selected->data);
        isEditMode = true;
        enter_edit_mode();
    } else {
        select_grid(nullptr);
        isEditMode = false;
    }

    // Updates all that is needed
    update_grid(scene->grids[0]);
    scene->colorSelected = 2;

    init_framebuffer();


/*     ShaderLib::uniform_int32(renderInfo.voxelProgram, "palette", 1);
    ShaderLib::uniform_int32(renderInfo.spriteProgram, "palette", 1); */
    selection.selectedCount = 0;
    selection.selection = nullptr;

    cacheSize = 32;
    cache = new GridCache[cacheSize];
    cacheDepth = 0;
    cacheIndex = 0;
}

void Viewport::init_framebuffer() {
    renderQuad = RenderLib::create_quad();

    framebuffer = TextureLib::create_framebuffer(Input.windowWidth, Input.windowHeight);
    uint32_t colorAttachment = TextureLib::create_texture_2d(GL_TEXTURE_2D, Input.windowWidth, Input.windowHeight, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    TextureLib::framebuffer_attachment(colorAttachment, GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);
    framebuffer.texture = colorAttachment;

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Input.windowWidth, Input.windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    framebuffer.depth = rboDepth;

    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    assert_msg(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer not complete!");

    ShaderLib::uniform_int32(renderInfo.voxelProgram, "grid", 0);
    if(selectedGrid != nullptr && scene->selected->type == OBJECT_TYPE_GRID) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, ((Grid*)scene->selected->data)->gridTexture);
    }
    WARNING("Test");
}



///
/// Updates
/// 
void Viewport::update() {
}

void Viewport::terminate() {
    
}

void Viewport::enter_edit_mode() {
    switch(scene->selected->type) {
        case OBJECT_TYPE_GRID: {
            viewportEditor = new VoxelGridEditor();

            viewportEditor->assign(scene->selected, scene, &info);
            viewportEditor->init();
            break;
        } case OBJECT_TYPE_SPRITE: {
            viewportEditor = new SpriteEditor();

            viewportEditor->assign(scene->selected, scene, &info);
            viewportEditor->init();
            break;
        } default: {
            ERROR("No edit mode for object of type " << scene->selected->type << " available!");
            break;
        }
    }
}

void Viewport::draw(WindowTileInfo tileInfo) {
    info.camera.update();

    solve_input();
    this->info.tileInfo = tileInfo;
    
    RenderLib::bind_framebuffer(framebuffer.framebuffer);
    RenderLib::update();

    if(isEditMode) {
        assert_msg(viewportEditor != nullptr, "You are trying to access edit mode, but no viewport editor is initialized");
        viewportEditor->update(renderInfo);
    }

    // Draw the scene graph
    draw_scene_object(&scene->sceneGraph);

    RenderLib::bind_framebuffer(0);
    RenderLib::update();

    // Draws resulting image into the ImGui window
    ImGui::GetWindowDrawList()->AddImage((void*)framebuffer.texture, 
                                        ImVec2(tileInfo.x * Input.windowWidth, tileInfo.y * Input.windowHeight), 
                                        ImVec2((tileInfo.x + tileInfo.width) * Input.windowWidth,
                                        (tileInfo.y + tileInfo.height) * Input.windowHeight), 
                                        ImVec2(0, 1), ImVec2(1, 0));

    draw_ui();

    #if DEPRECATED
    solve_input();
    this->tileInfo = tileInfo;

    // Capture mouse for camera, but only if tile is hovered
    if(ImGui::IsWindowHovered()) {
        camera.update();
        float offsetX = tileInfo.x * Input.windowWidth; 
        float offsetY = tileInfo.y * Input.windowHeight;
       
        if(Input.get(GLFW_KEY_DELETE) == KEY_STATE_PRESS) {
            memset(tempGrid.buffer, 0, selectedGrid->width * selectedGrid->depth * selectedGrid->height);
            memset(selectedGrid->buffer, 0, selectedGrid->width * selectedGrid->depth * selectedGrid->height);
            update_grid(*selectedGrid);
            MESSAGE("Delete");
        }

        // Switch edit mode
        if(Input.get(GLFW_KEY_TAB) == KEY_STATE_PRESS && selectedGrid != nullptr) {
            // Simple switch between normal & edit mode
            isEditMode = 1 - isEditMode;
            MESSAGE("Edit mode switch");
        }

        if(!isEditMode) {
            double cursorX, cursorY;
            Input.get_mapped_cursor(tileInfo, &cursorX, &cursorY);
            Ray ray = camera.create_ray(glm::vec3(cursorX, cursorY, 0.0f));
        } else {
            if(scene->selected != nullptr)
                solve_voxel_placing();

            if(Input.get(GLFW_KEY_LEFT_CONTROL) == KEY_STATE_HELD && Input.get(GLFW_KEY_LEFT_SHIFT) == KEY_STATE_NONE && Input.get(GLFW_KEY_Z) == KEY_STATE_PRESS) {
                undo();
            } if(Input.get(GLFW_KEY_LEFT_CONTROL) == KEY_STATE_HELD && Input.get(GLFW_KEY_LEFT_SHIFT) == KEY_STATE_HELD && Input.get(GLFW_KEY_Z) == KEY_STATE_PRESS) {
                redo();
            }
        }
    }

    #pragma region Rendering
    // Bounds the camera
    RenderLib::bind_framebuffer(framebuffer.framebuffer);
    RenderLib::update();

    //RenderLib::draw_scene(framebuffer, scene);
    RenderLib::draw_sky(renderInfo, camera.mode);
    // Draws the cage
    if(selectedGrid != nullptr && scene->selected != nullptr && isEditMode) {
        RenderLib::front_face(GL_CW);
        RenderLib::bind_vertex_array(renderInfo.voxelVAO);
        RenderLib::draw_voxel(renderInfo.boxProgram, scene->selected->transform.transform, glm::vec3(tempGrid.width, tempGrid.depth, tempGrid.height));
        RenderLib::front_face(GL_CCW);

        ShaderLib::program_use(renderInfo.voxelProgram);
        glm::vec3 camPos = camera.origin + camera.direction * -camera.offset;
        ShaderLib::uniform_vec3(renderInfo.voxelProgram, "cameraPos", &camPos.x);

        RenderLib::draw_grid(renderInfo, tempGrid, scene->selected->transform);
    } else {
        for(uint32_t i = 0; i < scene->gridCount; i++) {
            RenderLib::bind_vertex_array(renderInfo.voxelVAO);
            
            RenderLib::draw_grid(renderInfo, scene->grids[i], {glm::mat4(1.0f)});
            //RenderLib::draw_voxel(renderInfo.boxProgram, scene->selected->transform.transform, glm::vec3(scene->_grids[i].width, scene->_grids[i].depth, scene->_grids[i].height));
        }
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draws resulting image into the ImGui window
    ImGui::GetWindowDrawList()->AddImage((void*)framebuffer.texture, 
                                        ImVec2(tileInfo.x * Input.windowWidth, tileInfo.y * Input.windowHeight), 
                                        ImVec2((tileInfo.x + tileInfo.width) * Input.windowWidth,
                                        (tileInfo.y + tileInfo.height) * Input.windowHeight), 
                                        ImVec2(0, 1), ImVec2(1, 0));

    draw_ui();
    #pragma endregion

    #endif
}

void Viewport::draw_scene_object(const SceneObject* sceneObject) {

}

void Viewport::draw_ui() {
    ImGui::BeginChild("viewport_panel", ImVec2(ImVec2((info.tileInfo.x + info.tileInfo.width) * Input.windowWidth, 100)));
    if (ImGui::RadioButton("Add", brushMode == BRUSH_MODE_ADD)) { 
        brushMode = BRUSH_MODE_ADD; 
        brushModeCache = brushMode;
    } ImGui::SameLine();
    if (ImGui::RadioButton("Substract", brushMode == BRUSH_MODE_SUBSTRACT)) { 
        brushMode = BRUSH_MODE_SUBSTRACT;
        brushModeCache = brushMode;
    } ImGui::SameLine();
    if (ImGui::RadioButton("Paint", brushMode == BRUSH_MODE_PAINT)) { 
        brushMode = BRUSH_MODE_PAINT; 
        brushModeCache = brushMode;
    }
    ImGui::EndChild();
}

void Viewport::solve_voxel_placing() {
    // Initialize a new ray 
    double cursorX, cursorY;

    Input.get_mapped_cursor(info.tileInfo, &cursorX, &cursorY);

    Ray ray = info.camera.create_ray(glm::vec3(cursorX, cursorY, 0.0f));
    RayHit hit = ray_cast(ray);

    // If user want to draw a shape
    if(Input.get(GLFW_KEY_LEFT_SHIFT))
        drawMode = DRAW_MODE_SHAPE;
    else if(Input.get(GLFW_KEY_LEFT_ALT))
        drawMode = DRAW_MODE_EXTRUDE;
    else
        drawMode = DRAW_MODE_BRUSH;


    #pragma region SHAPE
    // If the shape is being drawn
    if(drawMode == DRAW_MODE_SHAPE) {
        if(!isDrawing && Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_PRESS) {
            isDrawing = true;
            shapeStart = hit.point + hit.normal;
        } if(isDrawing && Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_NONE) {
            isDrawing = false;

            
            shapeEnd = brushMode == BRUSH_MODE_ADD ? hit.point + hit.normal : hit.point;
            memcpy(tempGrid.buffer, selectedGrid->buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
            solve_rectangle(&tempGrid, shapeStart, shapeEnd);
            memcpy(selectedGrid->buffer, tempGrid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
            update_grid(*selectedGrid);

            shapeEnd = glm::vec3(0.0f);
            shapeStart = glm::vec3(0.0f);

            if(isEditMode)
                update_cache();
        } else if(isDrawing && Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_HELD) {
            glm::vec3 shapeEndTemp = brushMode == BRUSH_MODE_ADD ? hit.point + hit.normal : hit.point;
            if(std::floor(shapeEnd[0]) != std::floor(shapeEndTemp[0]) ||
                std::floor(shapeEnd[1]) != std::floor(shapeEndTemp[1]) ||
                std::floor(shapeEnd[2]) != std::floor(shapeEndTemp[2])) {

                memcpy(tempGrid.buffer, selectedGrid->buffer, tempGrid.width * tempGrid.depth * tempGrid.height);

                shapeEnd = shapeEndTemp;
                solve_rectangle(&tempGrid, shapeStart, shapeEnd);
                update_grid(tempGrid);
            }
        }
    } 
    #pragma endregion

    #pragma region BRUSH
    else if(drawMode == DRAW_MODE_BRUSH) {
        if(!Input.get(GLFW_KEY_LEFT_ALT) && Input.get(GLFW_MOUSE_BUTTON_1)) {
            glm::vec3 shapeEndTemp = brushMode == BRUSH_MODE_ADD ? hit.point + hit.normal : hit.point;

            if(std::floor(shapeEnd[0]) != std::floor(shapeEndTemp[0]) ||
                std::floor(shapeEnd[1]) != std::floor(shapeEndTemp[1]) ||
                std::floor(shapeEnd[2]) != std::floor(shapeEndTemp[2])) {

                shapeEnd = shapeEndTemp;

                uint32_t index = get_index(shapeEnd);
                if(selectedGrid->intersects(shapeEnd)) {
                    if(brushMode == BRUSH_MODE_ADD) {
                        tempCache.buffer[tempCache.count++] = {index, selectedGrid->get(index), scene->colorSelected};
                        tempGrid.set(index, scene->colorSelected);
                    } else if(brushMode == BRUSH_MODE_PAINT) {
                        if(tempGrid.get(index) > 0) {
                            tempCache.buffer[tempCache.count++] = {index, selectedGrid->get(index), scene->colorSelected};
                            tempGrid.set(index, scene->colorSelected);
                        }
                    } else if(brushMode == BRUSH_MODE_SUBSTRACT) {
                        tempCache.buffer[tempCache.count++] = {index, selectedGrid->get(index), 0};
                        tempGrid.set(index, 0);
                    }
                    update_grid(tempGrid);
                    requireUpdate = true;
                }
            }
        } else {
            if(!Input.get(GLFW_KEY_LEFT_ALT) && requireUpdate) {
                update_cache();
                memcpy(selectedGrid->buffer, tempGrid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
                update_grid(*selectedGrid);
                requireUpdate = false;
            }
        }
    }
    #pragma endregion

    #pragma region EXTRUSION
    if(drawMode == DRAW_MODE_EXTRUDE && Input.get(GLFW_MOUSE_BUTTON_1) && !isDrawing) {
        float distance = 0.0f;
        float step = 0.05f;


        while(distance < 240.0f) {
            distance += step;
            if(selectedGrid->get((ray.origin) + ray.direction * distance) > 0) {
                glm::vec3 position = ((ray.origin)) + ray.direction * distance;
                position.x = std::floor(position.x);
                position.y = std::floor(position.y);
                position.z = std::floor(position.z);

                glm::vec3 normal = ((ray.origin)) + (ray.direction * (distance - step));
                normal.x = std::floor(normal.x);
                normal.y = std::floor(normal.y);
                normal.z = std::floor(normal.z);

                // Realloc the selection
                // TODO No need to remove and then recreate
                if(selection.selection != nullptr)
                    delete [] selection.selection;
                selection.selection = new uint32_t[tempGrid.width * tempGrid.depth * tempGrid.height];
                memset(selection.selection, 0, sizeof(uint32_t) * (tempGrid.width * tempGrid.depth));
                
                normal = normal - position;
                normal = glm::normalize(normal);

                selection.selectedCount = 0;
                flood_fill(position, normal, scene->colorSelected);

                // Set some properties relating the shape
                shapeStart = position;
                shapeEnd = shapeStart;
                shapeNormal = normal;

                glm::vec3 n = shapeNormal * glm::vec3(1.0, tempGrid.width, tempGrid.width * tempGrid.depth);
                shapeNormalOffset = n.x + n.y + n.z;
                
                // Says that user started drawing
                isDrawing = true;

                update_grid(tempGrid);
                break;
            }
        }
    } else if(drawMode == DRAW_MODE_EXTRUDE && Input.get(GLFW_MOUSE_BUTTON_1) && isDrawing) {
        float result = 0.0f;

        glm::vec3 planeNormal = ray.origin - shapeStart;
        planeNormal = planeNormal * (glm::vec3(1.0f, 1.0f, 1.0f) - shapeNormal);
        glm::intersectRayPlane(ray.origin, ray.direction, shapeStart, planeNormal, result);
        glm::vec3 r = (ray.origin + ray.direction * result);

        shapeEnd = shapeStart + shapeNormal * result;

        memcpy(tempGrid.buffer, selectedGrid->buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
        float height = glm::length(r - shapeStart);
        if(glm::dot(shapeNormal, (r - shapeStart)) < 0.0f)
            height *= -1.0f;

        extrude(height);

        update_grid(tempGrid);
    } else if(drawMode == DRAW_MODE_EXTRUDE && Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_NONE && isDrawing) {
        isDrawing = false;
        memcpy(selectedGrid->buffer, tempGrid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
        update_grid(*selectedGrid);
        selection.selectedCount = 0;
    }
    #pragma endregion
}

void Viewport::extrude(int32_t height) {
    ERROR("Extrude " << height);
    if(height > 0) {
        for(uint32_t i = 0; i < selection.selectedCount; i++) {
            for(int32_t x = 0; x < height; x++) {
                tempGrid.set(selection.selection[i] + x * shapeNormalOffset, scene->colorSelected);
            }
        }
    } else {
        for(uint32_t i = 0; i < selection.selectedCount; i++) {
            for(int32_t x = 0; x > height; x--) {
                tempGrid.set(selection.selection[i] + x * shapeNormalOffset, 0);
            }
        }
    }
}

RayHit Viewport::ray_cast(Ray ray) {
    RayHit result;

    float distance = 0.0f;
    float step = 0.1f;

    while(distance < 240.0f) {
        distance += step;
        glm::vec3 offset;
        offset = glm::vec3(0.0f);
        if(selectedGrid->intersects((ray.origin + offset + ray.direction * distance))) {
            if(selectedGrid->get(ray.origin + offset + ray.direction * distance) > 0) {
                // Looks terrible
                // TODO Clean up
                //          - Probably putting the vector floor to separate function
                result.point = ray.origin + offset + ray.direction * (distance);
                
                glm::vec3 n = (ray.origin + offset + ray.direction * (distance - step));
                glm::vec3 point = glm::vec3(std::floor(result.point.x), std::floor(result.point.y), std::floor(result.point.z));
                glm::vec3 normalPoint = glm::vec3(std::floor(n.x), std::floor(n.y), std::floor(n.z));
                result.normal = glm::normalize(normalPoint - point);
                break;
            }
        } else {
            if(selectedGrid->intersects(ray.origin + offset + ray.direction * (distance - step))) {
                result.point = ray.origin + offset + ray.direction * (distance);
                
                glm::vec3 n = (ray.origin + offset + ray.direction * (distance - step));
                glm::vec3 point = glm::vec3(std::floor(result.point.x), std::floor(result.point.y), std::floor(result.point.z));
                glm::vec3 normalPoint = glm::vec3(std::floor(n.x), std::floor(n.y), std::floor(n.z));
                result.normal = glm::normalize(normalPoint - point);
                break;
            }
        }
    }

    return result;
}

void Viewport::flood_fill(glm::vec3 position, glm::vec3 normal, int8_t brush) {
    Grid* grid = selectedGrid;

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
        uint32_t index = (position + normal).x + (position + normal).y * (tempGrid.width) + (position + normal).z * (tempGrid.width * tempGrid.depth);
        for(uint32_t i = 0; i < selection.selectedCount; i++)
            if(selection.selection[i] == index)
                return;
        selection.selection[selection.selectedCount++] = index;

        flood_fill(position + right, normal, brush);
        flood_fill(position + forward, normal, brush);
        flood_fill(position - right, normal, brush);
        flood_fill(position - forward, normal, brush);
    }

    return;
}

void Viewport::solve_input() {
    // Switch edit mode
    if(Input.get(GLFW_KEY_TAB) == KEY_STATE_PRESS && selectedGrid != nullptr) {
        isEditMode = 1 - isEditMode;

        if(isEditMode) {
            enter_edit_mode();
        }
    }

    #pragma region ORTHOGRAPHIC VIEWS
    if(Input.get(GLFW_KEY_5) == KEY_STATE_PRESS) {
        MESSAGE("Changing mode");
        info.camera.set_mode(1 - info.camera.mode);
    } if(Input.get(GLFW_KEY_1) == KEY_STATE_PRESS) {
        MESSAGE("Front View");
        info.camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
        info.camera.direction = glm::vec3(0.0f, 1.0f, 0.0f);
    } if(Input.get(GLFW_KEY_3) == KEY_STATE_PRESS) {
        MESSAGE("Side View");
        info.camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
        info.camera.direction = glm::vec3(-1.0f, 0.0f, 0.0f);
    } if(Input.get(GLFW_KEY_7) == KEY_STATE_PRESS) {
        MESSAGE("Top View");
        info.camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
        info.camera.direction = glm::vec3(0.0f, 0.0f, -0.9f);
    } if(Input.get(GLFW_KEY_9) == KEY_STATE_PRESS) {
        MESSAGE("Opposite View");
        info.camera.direction *= glm::vec3(-1.0f);
    }
    #pragma endregion
}

void Viewport::solve_rectangle(Grid* grid, glm::vec3 start, glm::vec3 end) {
    switch(shapeMode) {
        case SHAPE_CUBE: {
            if(start.x > end.x) { std::swap(start.x, end.x); }
            if(start.y > end.y) { std::swap(start.y, end.y); }
            if(start.z > end.z) { std::swap(start.z, end.z); }

            uint32_t index = 0;
            for(float x = std::floor(start.x); x <= std::floor(end.x); x++) {
                for(float y = std::floor(start.y); y <= std::floor(end.y); y++) {
                    for(float z = std::floor(start.z); z <= std::floor(end.z); z++) {
                        if(brushMode == BRUSH_MODE_ADD) {
                            grid->set(glm::vec3(x, y, z), scene->colorSelected);
                        } if(brushMode == BRUSH_MODE_PAINT) {
                            if(grid->get(glm::vec3(x, y, z)) > 0)
                            grid->set(glm::vec3(x, y, z), scene->colorSelected);
                        } else if(brushMode == BRUSH_MODE_SUBSTRACT) {
                            grid->set(glm::vec3(x, y, z), 0);
                        }
                        requireUpdate = true;
                    }
                }
            }
            break;
        } case SHAPE_LINE: {
            glm::vec3 direction = end - start;
            float length = glm::length(direction);
            direction = glm::normalize(direction);

            float distance = 0.0f;
            float step = 0.1f;

            while(distance < length) {
                distance += step;

                if(brushMode == BRUSH_MODE_ADD) {
                    grid->set(start + direction * distance, scene->colorSelected);
                } else if(brushMode == BRUSH_MODE_PAINT) {
                    if(grid->get(start + direction * distance) > 0)
                        grid->set(start + direction * distance, scene->colorSelected);
                } else {
                    grid->set(start + direction * distance, 0);
                }
                requireUpdate = true;
            }
            break;
        }
    }
}

void Viewport::update_grid(Grid grid) {
    TextureLib::update_texture_3d(grid.gridTexture, grid.width, grid.depth, grid.height, grid.buffer);
}

void Viewport::update_cache() {
    if(cacheIndex > 0) {
        cacheDepth = 0;
    }

    cache[cacheDepth].init(tempCache.count);
    memcpy(cache[cacheDepth].buffer, tempCache.buffer, sizeof(GridCacheCell) * tempCache.count);
    cache[cacheDepth].count = tempCache.count;
    cacheDepth++;
    MESSAGE("Updating cache " << cacheDepth);

    tempCache.count = 0;
    cacheIndex = 0;
}

void Viewport::undo() {
    if(cacheDepth > cacheIndex) {
        MESSAGE("Undo " << cacheIndex << " for " << cache[cacheIndex].count);
        cacheIndex++;
        for(uint32_t i = 0; i < cache[cacheDepth - cacheIndex].count; i++) {
            tempGrid.set(cache[cacheDepth - cacheIndex].buffer[i].index, cache[cacheDepth - cacheIndex].buffer[i].color);
        }

        memcpy(selectedGrid->buffer, tempGrid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
        update_grid(*selectedGrid);
    }
}

void Viewport::redo() {
    if(cacheIndex > 0) {
        MESSAGE("Redo " << cacheIndex);
        for(uint32_t i = 0; i < cache[cacheDepth - cacheIndex].count; i++) {
            tempGrid.set(cache[cacheDepth - cacheIndex].buffer[i].index, cache[cacheDepth - cacheIndex].buffer[i].newColor);
        }
        cacheIndex--;

        memcpy(selectedGrid->buffer, tempGrid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
        update_grid(*selectedGrid);
    }
}


void Viewport::select_grid(uint32_t index) {
    if(index > scene->gridCount)
        return;

    MESSAGE("Changing grid to " << selectedGrid);
    // If the index says to unselect
    if(index == -1) {
        selectedGrid = nullptr;
        return;
    }

    selectIndex = index;

    // If there is something in cache
    // TODO might be cool to save it for future undo/redo :)
    if(tempGrid.buffer)
        delete [] tempGrid.buffer;

    tempGrid = scene->grids[index];
    selectedGrid = scene->grids + index;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, tempGrid.gridTexture);

    tempGrid.buffer = new int8_t[tempGrid.width * tempGrid.depth * tempGrid.height];
    memcpy(tempGrid.buffer, selectedGrid->buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
} void Viewport::select_grid(Grid* grid) {
    if(grid == nullptr) {
        MESSAGE("Deselecting");
        selectedGrid = nullptr;

        return;
    }

    // If there is something in cache
    // TODO might be cool to save it for future undo/redo :)
    if(tempGrid.buffer)
        delete [] tempGrid.buffer;

    tempGrid = *grid;
    selectedGrid = grid;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, tempGrid.gridTexture);

    tempGrid.buffer = new int8_t[tempGrid.width * tempGrid.depth * tempGrid.height];
    memcpy(tempGrid.buffer, selectedGrid->buffer, tempGrid.width * tempGrid.depth * tempGrid.height);

 /*    if(tempCache.buffer != nullptr)
        delete [] tempCache.buffer; */
    tempCache.init(tempGrid.width * tempGrid.depth * tempGrid.height);
}


void Viewport::resize_callback(uint32_t width, uint32_t height) {
    MESSAGE("Resizing Viewport");
    float aspect = (float)(Input.windowWidth * info.tileInfo.width) / (float)(Input.windowHeight * info.tileInfo.height);
    MESSAGE("Aspect: " << aspect);
    info.camera.resize_callback(Input.windowWidth * info.tileInfo.width, Input.windowHeight * info.tileInfo.height);
    framebuffer.width = Input.windowWidth;
    framebuffer.height = Input.windowHeight;

    glDeleteTextures(1, &framebuffer.texture);
    glDeleteRenderbuffers(1, &framebuffer.depth);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.framebuffer);

    uint32_t colorAttachment = TextureLib::create_texture_2d(GL_TEXTURE_2D, Input.windowWidth, Input.windowHeight, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    TextureLib::framebuffer_attachment(colorAttachment, GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);
    framebuffer.texture = colorAttachment;

    glGenRenderbuffers(1, &framebuffer.depth);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Input.windowWidth, Input.windowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depth);
}


void Viewport::refresh() {
    MESSAGE("Refreshing the viewport");
    if(scene->selected != nullptr) {
        select_grid((Grid*)scene->selected->data);

        update_grid(*selectedGrid);
        update_cache();
    } else {
        isEditMode = false;
        selectedGrid = nullptr;
    }
}

uint32_t Viewport::get_index(glm::vec3 pos) {
    ERROR("Index on :" << pos.x << "," << pos.y << "," << pos.z);
    return std::floor(pos.x) + std::floor(pos.y) * tempGrid.width + std::floor(pos.z) * (tempGrid.depth * tempGrid.width);
}