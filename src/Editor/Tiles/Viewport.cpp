#include "Viewport.h"

#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/intersect.hpp>
#include <avg/Debug.h>
#include <chrono>
#include "ImGui/imgui.h"
#include "Rendering/TextureLib.h"
#include "Editor/Viewport/SpriteEditor.h"
#include "System/Math.h"

typedef std::chrono::high_resolution_clock Clock;


///
/// Initialization
///
void Viewport::init() {
    // Initializes few things
    info.camera.init();
    info.viewport = this;

    // Set some default values
    snapping = false;
    snappingSwitch = 0;
    transformMode = TRANSFORM_MODE_NONE;
    viewportEditor = nullptr;
    isEditMode = 0;

    init_framebuffer();
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

    double cursorX, cursorY;
    Input.get_mapped_cursor(tileInfo, &cursorX, &cursorY);

    if(isEditMode) {
        assert_msg(viewportEditor != nullptr, "You are trying to access edit mode, but no viewport editor is initialized");
        viewportEditor->draw(renderInfo, tileInfo);

        // Only update when user hovers the viewport
        if(cursorX > -1.0 && cursorX < 1.0 &&
        cursorY > -1.0 && cursorY < 1.0) {
            viewportEditor->update(renderInfo);
        }
    } else {
        RenderLib::draw_sky(renderInfo, info.camera.mode);
        draw_scene_object(&scene->sceneGraph);

        if(Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_PRESS) {
            WARNING("Selecting");
            select_scene_object(&scene->sceneGraph);
        }
    }

    RenderLib::bind_framebuffer(0);
    RenderLib::update();

    // Draws resulting image into the ImGui window
    ImGui::GetWindowDrawList()->AddImage((void*)framebuffer.texture, 
                                        ImVec2(tileInfo.x * Input.windowWidth, tileInfo.y * Input.windowHeight), 
                                        ImVec2((tileInfo.x + tileInfo.width) * Input.windowWidth,
                                        (tileInfo.y + tileInfo.height) * Input.windowHeight), 
                                        ImVec2(0, 1), ImVec2(1, 0));
                                        
    ImGui::BeginChild("tool_bar", ImVec2(500, tileInfo.height));
    if(viewportEditor) {
        viewportEditor->tool_bar();
    }

    ImGui::EndChild();

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

bool Viewport::select_scene_object(SceneObject* sceneObject) {
    WARNING(sceneObject->name << " " << sceneObject->childrenCount << " " << sceneObject->type);
    switch(sceneObject->type) {
        case OBJECT_TYPE_GRID: {
            SceneGrid* sceneGrid = (SceneGrid*)sceneObject->data;

            double cursorX, cursorY;
            Input.get_mapped_cursor(info.tileInfo, &cursorX, &cursorY);

            Ray ray = info.camera.create_ray(glm::vec3(cursorX, cursorY, 0.0f));
            if(Math::ray_box_intersection(&ray, glm::vec3(0.0f), glm::vec3(sceneGrid->width, sceneGrid->depth, sceneGrid->height))) {
                if(sceneObject != scene->selected) {
                    scene->selected = sceneObject;
                    return true;
                } else {
                    ERROR("Already selected " << sceneObject->name);
                }
            }

            break;
        } case OBJECT_TYPE_SPRITE: {
            double cursorX, cursorY;
            Input.get_mapped_cursor(info.tileInfo, &cursorX, &cursorY);

            Ray ray = info.camera.create_ray(glm::vec3(cursorX, cursorY, 0.0f));
            if(Math::ray_box_intersection(&ray, glm::vec3(0.0f), glm::vec3(32.0f, 1.0f, 32.0f))) {
                if(sceneObject != scene->selected) {
                    MESSAGE("Selected " << sceneObject->name);
                    scene->selected = sceneObject;
                    return true;
                } else {
                    ERROR("Already selected " << sceneObject->name);
                }
            } else {
                ERROR("Nope");
            }
            break;
        } default: {
            ERROR("Not a grid od sprite!");
            break;
        }
    }

    for(uint32_t i = 0; i < sceneObject->childrenCount; i++) {
        if(select_scene_object(sceneObject->children + i)) {
            return true;
        }
    }
}

void Viewport::draw_scene_object(const SceneObject* sceneObject) {
    RenderLib::bind_vertex_array(renderInfo.voxelVAO);

    glm::vec3 size = glm::vec3(0.0f);
    switch(sceneObject->type) {
        case OBJECT_TYPE_GRID: {
            size = glm::vec3(((SceneGrid*)sceneObject->data)->width, ((SceneGrid*)sceneObject->data)->depth, ((SceneGrid*)sceneObject->data)->height);
            break;
        } case OBJECT_TYPE_SPRITE: {
            size = glm::vec3(((SceneSprite*)sceneObject->data)->width, 1.0f, ((SceneSprite*)sceneObject->data)->height);
            break;
        }
    }

    RenderLib::draw_voxel(renderInfo.boxProgram, scene->selected->transform.transform, size);

    for(uint32_t i = 0; i < sceneObject->childrenCount; i++) {
        draw_scene_object(&sceneObject->children[i]);
    }
}

void Viewport::draw_ui() {

}

void Viewport::extrude(int32_t height) {
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
    if(Input.get(GLFW_KEY_TAB) == KEY_STATE_PRESS) {
        if(selectedGrid != nullptr) {
            isEditMode = 1 - isEditMode;

            if(isEditMode) {
                enter_edit_mode();
            } else {
                info.camera.flags = CAMERA_ALLOW_PANNING | CAMERA_ALLOW_ROTATION | CAMERA_ALLOW_ZOOMING;
            }
        } else {
            ERROR("You are trying to enter edit mode, while nothing is selected!");
        }
    }

    if(Input.get(GLFW_KEY_LEFT_CONTROL) == KEY_STATE_HELD && Input.get(GLFW_KEY_Z) == KEY_STATE_PRESS && Input.get(GLFW_KEY_LEFT_SHIFT) == KEY_STATE_HELD) {
        redo();
    } else if(Input.get(GLFW_KEY_LEFT_CONTROL) == KEY_STATE_HELD && Input.get(GLFW_KEY_Z) == KEY_STATE_PRESS) {
        undo();
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
    viewportEditor->undo();
}

void Viewport::redo() {
    viewportEditor->redo();
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

}