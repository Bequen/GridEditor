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
typedef std::chrono::high_resolution_clock Clock;


///
/// Initialization
///
void Viewport::init() {
    // Initializes few things
    camera.init();

    // Set some default values
    snapping = false;
    snappingSwitch = 0;
    isEditMode = 1;
    isDrawing = false;
    drawMode = DRAW_MODE_BRUSH;
    brushMode = BRUSH_MODE_ADD;
    brushModeCache = BRUSH_MODE_ADD;
    shapeMode = SHAPE_CUBE;
    transformMode = TRANSFORM_MODE_NONE;

    if(scene->gridCount > 0)
        select_grid(0);
    else
        select_grid(-1);

    // Updates all that is needed
    update_grid(scene->grids[0]);
    update_cache();
    scene->colorSelected = 2;

    init_framebuffer();


    ShaderLib::uniform_int32(renderInfo.voxelProgram, "palette", 1);
    selection.selectedCount = 0;
    selection.selection = nullptr;
}

void Viewport::init_framebuffer() {
    renderQuad = RenderLib::create_quad();

    framebuffer = TextureLib::create_framebuffer(Input.windowWidth, Input.windowHeight);
    uint32_t colorAttachment = TextureLib::create_texture_2d(GL_TEXTURE_2D, Input.windowWidth, Input.windowHeight, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA, nullptr);
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

void Viewport::draw(WindowTileInfo tileInfo) {
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

            // Selecting 
            if(Input.get(GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
                for(uint32_t i = 0; i < scene->gridCount; i++) {
                    float result = 0.0f;
                    glm::intersectRaySphere(ray.origin, ray.direction, glm::vec3(scene->grids[i].width, scene->grids[i].depth, scene->grids[i].height) / 2.0f, 16.0f * 16.0f, result);

                    if(result != 0.0f) {
                        MESSAGE(scene->selected->name);
                        select_grid(i);
                    }
                }
            } 

            /* if(Input.get(GLFW_KEY_G) == GLFW_PRESS) {
                transformMode = TRANSFORM_MODE_TRANSLATE;
            } */
        } else {
            if(scene->selected != nullptr && isEditMode)
                solve_voxel_placing();
            /* if(input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_HELD) {
                Brush brush;
                brush.colorID = scene->colorSelected;
                editor->solve_brush(camera.create_ray(glm::vec3(cursor.cursorX, cursor.cursorY, 0.0f)), brush);
            }
            editor->update(input); */
        }
    }

    // Transform
    if(transformMode == TRANSFORM_MODE_TRANSLATE) {
        if(Input.get(GLFW_MOUSE_BUTTON_1)) {
            transformMode = TRANSFORM_MODE_NONE;
        }
        glm::vec3 translation;
        scene->selected->transform.transform = glm::translate(scene->selected->transform.transform, glm::vec3(-Input.mouseDeltaX * 10.0f, -Input.mouseDeltaY * 10.0f, 0.0f));
    }

    if(Input.get(GLFW_KEY_LEFT_CONTROL) && !snappingSwitch) {
        snappingSwitch = true;
    } else if(!Input.get(GLFW_KEY_LEFT_CONTROL) && snappingSwitch) {
        snappingSwitch = false;
    }

    // Bounds the camera
    RenderLib::buffer_binding_range(camera.cameraBuffer, 0, 0, sizeof(glm::mat4) * 2);
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
        RenderLib::draw_grid(renderInfo, tempGrid, scene->selected->transform);
    } else {
        for(uint32_t i = 0; i < scene->gridCount; i++) {
            RenderLib::bind_vertex_array(renderInfo.voxelVAO);
            
            RenderLib::draw_grid(renderInfo, scene->grids[i], scene->selected->transform);
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
}

void Viewport::solve_voxel_placing() {
    // Initialize a new ray 
    double cursorX, cursorY;

    Input.get_mapped_cursor(tileInfo, &cursorX, &cursorY);

    Ray ray = camera.create_ray(glm::vec3(cursorX, cursorY, 0.0f));
    RayHit hit = ray_cast(ray);

    // If the user wants to delete stuff
    if(Input.get(GLFW_KEY_LEFT_CONTROL) == KEY_STATE_HELD) {
        scene->colorSelected = 0;

        this->brushMode = BRUSH_MODE_SUBSTRACT;
    } else {
        scene->colorSelected = scene->colorCache;

        this->brushMode = this->brushModeCache;
        MESSAGE("Brush mode : " << this->brushMode);
    }



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
            MESSAGE("Shape start: " << shapeStart.x << "," << shapeStart.y << "," << shapeStart.z);
        } if(isDrawing && Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_NONE) {
            isDrawing = false;

            shapeEnd = hit.point + hit.normal;
            MESSAGE("Shape End: " << shapeEnd.x << "," << shapeEnd.y << "," << shapeEnd.z);
            memcpy(selectedGrid->buffer, tempGrid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
            solve_rectangle(selectedGrid, shapeStart, shapeEnd);
            update_grid(*selectedGrid);

            if(isEditMode)
                update_cache();
        } else if(isDrawing && Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_HELD) {
            glm::vec3 shapeEndTemp = hit.point + hit.normal;
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

            //ERROR("Drawing at " << shapeEndTemp.x << "|" << shapeEndTemp.y << "|" << shapeEndTemp.z);
            if(std::floor(shapeEnd[0]) != std::floor(shapeEndTemp[0]) ||
                std::floor(shapeEnd[1]) != std::floor(shapeEndTemp[1]) ||
                std::floor(shapeEnd[2]) != std::floor(shapeEndTemp[2])) {

                shapeEnd = shapeEndTemp;

                if(selectedGrid->intersects(shapeEnd)) {
                    tempGrid.set(shapeEnd, scene->colorSelected);
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
    if(Input.get(GLFW_KEY_5) == KEY_STATE_PRESS) {
        MESSAGE("Changing mode");
        camera.set_mode(1 - camera.mode);
    } if(Input.get(GLFW_KEY_1) == KEY_STATE_PRESS) {
        MESSAGE("Front View");
        camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
        camera.direction = glm::vec3(0.0f, 1.0f, 0.0f);
    } if(Input.get(GLFW_KEY_3) == KEY_STATE_PRESS) {
        MESSAGE("Side View");
        camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
        camera.direction = glm::vec3(-1.0f, 0.0f, 0.0f);
    } if(Input.get(GLFW_KEY_7) == KEY_STATE_PRESS) {
        MESSAGE("Top View");
        camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
        camera.direction = glm::vec3(0.0f, 0.0f, -0.9f);
    } if(Input.get(GLFW_KEY_9) == KEY_STATE_PRESS) {
        MESSAGE("Opposite View");
        camera.direction *= glm::vec3(-1.0f);
    }
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
                        grid->set(glm::vec3(x, y, z), scene->colorSelected);
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

                grid->set(start + direction * distance, scene->colorSelected);
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
}


void Viewport::resize_callback(uint32_t width, uint32_t height) {
    /* if(tileInfo.width == 0.0)
        tileInfo.width = 1.0f;
    if(tileInfo.height == 0.0f)
        tileInfo.height = 1.0f; */

    float aspect = (float)(width * tileInfo.width) / (float)(height * tileInfo.height);
    camera.resize_callback(width * tileInfo.width,height * tileInfo.height);
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


void Viewport::enter_edit_mode() {
    isEditMode = 1;
} void Viewport::leave_edit_mode() {
    isEditMode = 0;
}

void Viewport::refresh() {
    MESSAGE("Refreshing the viewport");
    update_grid(*selectedGrid);
    update_cache();
}