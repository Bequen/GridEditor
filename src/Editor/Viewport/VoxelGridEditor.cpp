#include "VoxelGridEditor.h"

#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"

#include <cstring>
#include <avg/Debug.h>
#include <Rendering/TextureLib.h>
#include <glm/gtx/intersect.hpp>
#include <imgui/imgui.h>

#include "Editor/Tiles/Viewport.h"


void VoxelGridEditor::assign(const SceneObject* sceneObject, Scene* scene, ViewportInfo* viewport) {
    this->grid = (SceneGrid*)sceneObject->data;
    this->scene = scene;
    this->viewport = viewport;

    // Copies current grid to the temp one
    this->tempGrid = *this->grid;
    this->tempGrid.dup_buffer();
    //this->tempGrid.new_buffer();
    //this->tempGrid.set_buffer(new uint8_t[tempGrid.width * tempGrid.depth * tempGrid.height]);
    //memset(this->tempGrid.grid.buffer, 0, tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height);
    //memcpy(this->tempGrid.grid.buffer, grid->grid.buffer, grid->width * grid->height * grid->depth);
}

void VoxelGridEditor::init() {
    drawMode = DRAW_MODE_BRUSH;
    brushMode = BRUSH_MODE_ADD;
    isDrawing = false;
    requireUpdate = false;

    update_cache();
    extrusion.count = 0;
    extrusion.indices = 0;
}

void VoxelGridEditor::update(RenderInfo renderInfo) {
    /* if(grid->width != tempGrid.width ||
        grid->height != tempGrid.height ||
        grid->depth != tempGrid.depth) 
    {
        MESSAGE("Resizing temp grid");
        delete [] tempGrid.grid.buffer;
        tempGrid = *grid;

        this->tempGrid.grid.buffer = new uint8_t[tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height];
        memcpy(this->tempGrid.grid.buffer, grid->grid.buffer, grid->width * grid->height * grid->depth);
    } */

   /*  if(Input.get(GLFW_KEY_DELETE)) {
        if(selection.selectedCount == 0) {
            memset(grid->grid.buffer, 0, grid->width * grid->height * grid->depth);
            memset(tempGrid.grid.buffer, 0, grid->width * grid->height * grid->depth);

            tempGrid.update_texture();
        } else {
            for(uint32_t i = 0; i < selection.selectedCount; i++) {
                grid->set(selection.selection[i], 0);
            }
        }
    } */

    viewport->camera.update();
    solve_voxel_drawing();
}

void VoxelGridEditor::draw(RenderInfo renderInfo, WindowTileInfo tileInfo) {
    /* tempGrid.voxelGrid.texture = grid->voxelGrid.texture;
    
    tempGrid.grid.width = grid->grid.width;
    tempGrid.grid.height = grid->grid.height;
    tempGrid.grid.depth = grid->grid.depth; */

    RenderLib::draw_sky(renderInfo, viewport->camera.mode);

    RenderLib::front_face(GL_CW);
    RenderLib::bind_vertex_array(renderInfo.voxelVAO);
    RenderLib::draw_voxel(renderInfo.boxProgram, scene->selected->transform.transform, glm::vec3(grid->width, grid->depth, grid->height));
    RenderLib::front_face(GL_CCW);

    ShaderLib::program_use(renderInfo.voxelProgram);
    glm::vec3 camPos = viewport->camera.origin + viewport->camera.direction * -viewport->camera.offset;
    ShaderLib::uniform_vec3(renderInfo.voxelProgram, "cameraPos", &camPos.x);

    draw_grid(renderInfo, &tempGrid);
}

void VoxelGridEditor::draw_grid(RenderInfo renderInfo, const SceneGrid* grid) {
    uint32_t streak = 0;
    uint32_t streakBegin = 0;

/*     glUseProgram(renderInfo.voxelProgram);
    
    glBindVertexArray(renderInfo.voxelVAO);
    glUniform3ui(glGetUniformLocation(renderInfo.voxelProgram, "size"), grid->width, grid->depth, grid->height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, grid->voxelGrid.texture);
 */
    glBindVertexArray(renderInfo.voxelVAO);

    glUseProgram(renderInfo.voxelSelectedProgram);
    renderInfo.currentProgram = renderInfo.voxelSelectedProgram;
    // grid->draw_selected(renderInfo);

    /* uint32_t positionUniform = glGetUniformLocation(renderInfo.voxelSelectedProgram, "index");

    glUniform3ui(glGetUniformLocation(renderInfo.voxelSelectedProgram, "size"), grid->width, grid->depth, grid->height);
    glUniform1i(glGetUniformLocation(renderInfo.voxelSelectedProgram, "lightCount"), scene->lightCount);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, grid->voxelGrid.texture);

    for(uint32_t i = 0; i < selection.selectedCount; i++) {
        if(tempGrid.get(selection.selection[i]) > 0) {
            glUniform1i(positionUniform, selection.selection[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    }
 */
    glUseProgram(renderInfo.voxelProgram);
    glUniform1i(glGetUniformLocation(renderInfo.voxelProgram, "lightCount"), scene->lightCount);
    renderInfo.currentProgram = renderInfo.voxelProgram;
    grid->draw(renderInfo);

    /* glUseProgram(renderInfo.voxelProgram);
    positionUniform = glGetUniformLocation(renderInfo.voxelProgram, "index");
    glUniform1i(glGetUniformLocation(renderInfo.voxelProgram, "lightCount"), scene->lightCount);

    for(uint32_t i = 0; i < grid->grid.width * grid->grid.depth * grid->grid.height; i++) {
        if(grid->get(i) > 0) {
            streak++;
        } else {
            if(streak > 0) {
                glUniform1i(positionUniform, streakBegin);
                glDrawArraysInstanced(GL_TRIANGLES, 0, 36, streak);
                streakBegin += streak; streak = 0;
            }
            streakBegin++;
        }
    } */
}

void VoxelGridEditor::terminate() {
    
}


void VoxelGridEditor::resize_callback() {

}

void VoxelGridEditor::refresh_callback() {

}


void VoxelGridEditor::undo() {
    /* tempGrid.undo();

    memcpy(grid->grid.buffer, tempGrid.grid.buffer, tempGrid.width * tempGrid.height * tempGrid.depth);
    grid->update_texture(); */
}

void VoxelGridEditor::redo() {
    /* tempGrid.redo();

    memcpy(grid->grid.buffer, tempGrid.grid.buffer, tempGrid.width * tempGrid.height * tempGrid.depth);
    grid->update_texture(); */
}

void VoxelGridEditor::solve_voxel_drawing() {
    #pragma region Value preparation
    if(Input.get(GLFW_KEY_B) == KEY_STATE_PRESS) {
        drawMode = DRAW_MODE_SELECTION;
        selectMode = SELECT_MODE_BOX;
        brushMode = BRUSH_MODE_PAINT;
    }

    if(Input.get(GLFW_KEY_A) == KEY_STATE_PRESS) {
        /* if(tempGrid.selections.selectedCount > 0) {
            tempGrid.selections.selectedCount = 0;
        } else {
            tempGrid.selections.selection = new uint32_t[tempGrid.width * tempGrid.depth * tempGrid.height];
            for(uint32_t i = 0; i < tempGrid.width * tempGrid.depth * tempGrid.height; i++) {
                tempGrid.selection.selection[tempGrid.selection.selectedCount++] = i;
            }
        } */
        if(tempGrid.is_any_selected())
            tempGrid.unselect_all();
        else
            tempGrid.select_all();
    }

    if(drawMode != DRAW_MODE_SELECTION) {
        if(Input.get(GLFW_KEY_LEFT_SHIFT) == KEY_STATE_HELD)
            drawMode = DRAW_MODE_SHAPE;
        else if(Input.get(GLFW_KEY_LEFT_ALT) == KEY_STATE_HELD)
            drawMode = DRAW_MODE_EXTRUDE;
        else
            drawMode = DRAW_MODE_BRUSH;

        if(brushMode != BRUSH_MODE_PAINT) {
            if(Input.get(GLFW_KEY_LEFT_CONTROL) == KEY_STATE_PRESS) {
                brushMode = 1 - brushMode;
            } else if(Input.get(GLFW_KEY_LEFT_CONTROL) == KEY_STATE_RELEASE) {
                brushMode = 1 - brushMode;
            }
        }
    }

    double cursorX, cursorY;

    Input.get_mapped_cursor(viewport->tileInfo, &cursorX, &cursorY);

    Ray ray = viewport->camera.create_ray(glm::vec3(cursorX, cursorY, 0.0f));
    RayHit hit = ray_cast(ray);

    shape.end = brushMode == BRUSH_MODE_ADD ? hit.point + hit.normal : hit.point;
    if(brushMode == BRUSH_MODE_SUBSTRACT)
        brushColor = 0;
    else
        brushColor = scene->colorSelected;
    #pragma endregion

    if( std::floor(shape.end[0]) != std::floor(shapeEndTemp[0]) ||
        std::floor(shape.end[1]) != std::floor(shapeEndTemp[1]) ||
        std::floor(shape.end[2]) != std::floor(shapeEndTemp[2])) { 
        
        //shape.end = shapeEndTemp;
        requireUpdate = true;
    }

    uint32_t index = tempGrid.index(shape.end);

    if(drawMode == DRAW_MODE_BRUSH) {
        if(Input.get(GLFW_MOUSE_BUTTON_1) != KEY_STATE_NONE) {
            if(isDrawing) {
                if(brushMode == BRUSH_MODE_PAINT) {
                    if(tempGrid.get(shape.end) > 0) {
                        tempGrid.set(shape.end, brushColor);
                    }
                } else {
                    tempGrid.set(shape.end, brushColor);
                }
                requireUpdate = true;
                tempGrid.update_texture();
            } else {
                isDrawing = true;
            }
        } else {
            if(isDrawing) {
                update_cache();
                isDrawing = false;
                //memcpy(grid->grid.buffer, tempGrid.grid.buffer, tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height);
                grid->copy_buffer(&tempGrid);
                grid->update_texture();
                requireUpdate = false;
            }
        }
    } else if(drawMode == DRAW_MODE_SHAPE) {
        if(Input.get(GLFW_MOUSE_BUTTON_1) != KEY_STATE_NONE) {
            if(isDrawing) {
                tempGrid.copy_buffer(grid);
                //memcpy(tempGrid.grid.buffer, grid->grid.buffer, tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height);

                solve_shape(&tempGrid, shape.start, shape.end);
                //update_grid(&_tempGrid);
                requireUpdate = true;
                tempGrid.update_texture();
            } else {
                isDrawing = true;
                shape.start = shape.end;
            }
        } else {
            if(isDrawing) {
                isDrawing = false;

                solve_shape(&tempGrid, shape.start, shape.end);
                update_cache();
                //memcpy(grid->grid.buffer, tempGrid.grid.buffer, tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height);
                //update_grid(grid);
                grid->copy_buffer(&tempGrid);
                grid->update_texture();
                requireUpdate = false;
            }
        }
    } else if(drawMode == DRAW_MODE_EXTRUDE) {
        MESSAGE("extruding");
        if(Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_HELD) {
            if(isDrawing) {
                float result = 0.0f;

                glm::vec3 planeNormal = glm::normalize(ray.origin - shape.start);
                planeNormal = planeNormal * (glm::vec3(1.0f, 1.0f, 1.0f) - glm::normalize(shape.normal));
                glm::intersectRayPlane(ray.origin, ray.direction, shape.start, glm::normalize(planeNormal), result);
                glm::vec3 r = (ray.origin + ray.direction * result);

                //memcpy(tempGrid.grid.buffer, grid->grid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
                tempGrid.copy_buffer(grid);
                float height = glm::distance(r * shape.normal, shape.start * shape.normal);
                if(glm::dot(shape.normal, (r - shape.start)) < 0.0f)
                    height *= -1.0f;

                extrude(height);

                //update_grid(&tempGrid);
                tempGrid.update_texture();
            } else {
                tempGrid.clear_selection();

                shape.start = shape.end - hit.normal;
                //selection.selection = new uint32_t[tempGrid.width * tempGrid.depth * tempGrid.height];
                //memset(selection.selection, 0, sizeof(uint32_t) * (tempGrid.width * tempGrid.depth * tempGrid.height));
                //selection.selectedCount = 0;
                
                if(extrusion.indices) {
                    delete [] extrusion.indices;
                } 
                extrusion.indices = new uint32_t[tempGrid.width * tempGrid.depth];
                extrusion.count = 0;

                flood_fill(floor_vec(shape.start), hit.normal);

                shape.normal = hit.normal;
                isDrawing = true;
            }
        } else {
            if(isDrawing) {
                isDrawing = false;
                //memcpy(grid->grid.buffer, tempGrid.grid.buffer, tempGrid.width * tempGrid.depth * tempGrid.height);
                grid->copy_buffer(&tempGrid);
                //update_grid(grid);
                grid->update_texture();
                //selection.selectedCount = 0;
            }
        }
    } else if(drawMode == DRAW_MODE_SELECTION) {
        if(selectMode == SELECT_MODE_BOX) {
            if(Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_HELD) {
                if(isDrawing) {
                    tempGrid.clear_selection();
                    // selection.selectedCount = 0;
                    select_shape(&tempGrid, shape);
                } else {
                    tempGrid.clear_selection();
                    //selection.selection = new uint32_t[tempGrid.width * tempGrid.depth * tempGrid.height];
                    isDrawing = true;
                    shape.start = shape.end;
                }
            } else {
                if(isDrawing) {
                    update_cache();
                    drawMode = DRAW_MODE_BRUSH;
                    brushMode = BRUSH_MODE_ADD;
                    isDrawing = false;
                }
            }
        }
    }
}

void VoxelGridEditor::extrude(int32_t height) {
    //WARNING(height << " " << selection.selectedCount);
    int32_t shapeNormalOffset = grid->index(shape.normal);
    // shapeNormalOffset = 16*16;
   /*  if(height > 0) {
        for(uint32_t i = 0; i < tempGrid.selectedCount; i++) {
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
    } */

    tempGrid.extrude(&extrusion, shapeNormalOffset, height, scene->colorSelected);
}

void VoxelGridEditor::solve_shape(SceneGrid* grid, glm::vec3 start, glm::vec3 end) {
    switch(shapeMode) {
        case SHAPE_CUBE: {
            /* if(start.x > end.x) { std::swap(start.x, end.x); }
            if(start.y > end.y) { std::swap(start.y, end.y); }
            if(start.z > end.z) { std::swap(start.z, end.z); }

            //uint32_t index = 0;
            for(float x = std::floor(start.x); x <= std::floor(end.x); x++) {
                for(float y = std::floor(start.y); y <= std::floor(end.y); y++) {
                    for(float z = std::floor(start.z); z <= std::floor(end.z); z++) {
                        if(brushMode == BRUSH_MODE_ADD) {
                            grid->set(glm::vec3(x, y, z), brushColor);
                        } if(brushMode == BRUSH_MODE_PAINT) {
                            if(grid->get(glm::vec3(x, y, z)) > 0)
                            grid->set(glm::vec3(x, y, z), brushColor);
                        } else if(brushMode == BRUSH_MODE_SUBSTRACT) {
                            grid->set(glm::vec3(x, y, z), 0);
                        }
                        requireUpdate = true;
                    }
                }
            } */

            tempGrid.fill(start, end, brushMode, brushColor);
            requireUpdate = true;

            /* uint32_t index = start.x + start.y * 32 + start.z * (32 * 32);
            for(uint32_t z = start.z; z <= end.z; z++) {
                for(uint32_t y = start.y; y <= end.y; y++) {
                    for(uint32_t x = start.x; x <= end.x; x++) {
                        // memset(grid->grid.buffer + index, brushColor, (end.x - start.x));
                        //grid->grid.buffer[index + x] = brushColor;
                        grid->set(index + x, brushColor);
                    }
                    //index += grid->width - (end.x) + start.x;
                }
                //index += grid->depth - (end.y) + start.y;
            } */
            
            break;
        }
    }
}

void VoxelGridEditor::select_shape(SceneGrid* grid, Shape shape) {
    switch(selectMode) {
        case SELECT_MODE_BOX: {
            if(shape.start.x > shape.end.x) { std::swap(shape.start.x, shape.end.x); }
            if(shape.start.y > shape.end.y) { std::swap(shape.start.y, shape.end.y); }
            if(shape.start.z > shape.end.z) { std::swap(shape.start.z, shape.end.z); }

            uint32_t index = 0;
            for(float x = std::floor(shape.start.x); x <= std::floor(shape.end.x); x++) {
                for(float y = std::floor(shape.start.y); y <= std::floor(shape.end.y); y++) {
                    for(float z = std::floor(shape.start.z); z <= std::floor(shape.end.z); z++) {
                        // uint32_t index = tempGrid.index(glm::vec3(x, y, z));
                        /* if(grid->get(index) > 0) { */
                            //selection.selection[selection.selectedCount++] = index;
                            tempGrid.select(glm::vec3(x, y, z));
                        /* } */
                    }
                }
            }
            
            break;
        }
    }
}

RayHit VoxelGridEditor::ray_cast(const Ray& ray) {
    RayHit result;

    float distance = 0.0f;
    float step = 0.1f;

    while(distance < 240.0f) {
        distance += step;
        glm::vec3 offset;
        offset = glm::vec3(0.0f);
        if(grid->intersects((ray.origin + offset + ray.direction * distance))) {
            if(grid->get(ray.origin + offset + ray.direction * distance) > 0) {
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
            if(grid->intersects(ray.origin + offset + ray.direction * (distance - step))) {
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

uint32_t VoxelGridEditor::get_index(glm::vec3 position) {
    //return std::floor(position.x) + std::floor(position.y) * tempGrid.grid.width + std::floor(position.z) * (tempGrid.grid.depth * tempGrid.grid.width);
}

void VoxelGridEditor::update_grid(const Grid* grid) {
    TextureLib::update_texture_3d(grid->gridTexture, grid->width, grid->depth, grid->height, grid->buffer);
}

void VoxelGridEditor::flood_fill(glm::vec3 position, glm::vec3 normal) {
    if(position.x < 0 || position.x >= tempGrid.width ||
        position.y < 0 || position.y >= tempGrid.depth ||
        position.z < 0 || position.z >= tempGrid.height) {
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
        uint32_t index = tempGrid.index(position);
        /* for(uint32_t i = 0; i < selection.selectedCount; i++)
            if(selection.selection[i] == index)
                return; */

        if(tempGrid.is_selected(position)) {
            return;
        }

        tempGrid.select(position);
        extrusion.indices[extrusion.count++] = index;

        //selection.selection[selection.selectedCount++] = index;

        flood_fill(position + right, normal);
        flood_fill(position + forward, normal);
        flood_fill(position - right, normal);
        flood_fill(position - forward, normal);
    }

    return;
}

glm::vec3 VoxelGridEditor::floor_vec(glm::vec3 vec) {
    return glm::vec3(std::floor(vec.x), std::floor(vec.y), std::floor(vec.z));
}

void VoxelGridEditor::update_cache() {
    /* if(tempGrid.cacheIndex > 0) {
        tempGrid.cacheDepth = -1;
        tempGrid.cacheIndex = 0;
    }

    tempGrid.cache[++tempGrid.cacheDepth].count = 0;
    WARNING("Updating cache " << tempGrid.cacheDepth); */
    tempGrid.update_cache();
}


void VoxelGridEditor::side_bar() {

}

void VoxelGridEditor::menu_bar() {
    
}

void VoxelGridEditor::tool_bar() {
    ImGui::BeginChild("tool_bar_brush_mode", ImVec2(24, 80));
    if(ImGui::Button("+", ImVec2(24, 24))) {
        brushMode = BRUSH_MODE_ADD;
    }
    if(ImGui::Button("-", ImVec2(24, 24))) {
        brushMode = BRUSH_MODE_SUBSTRACT;
    }
    if(ImGui::Button("=", ImVec2(24, 24))) {
        brushMode = BRUSH_MODE_PAINT;
    }

    ImGui::EndChild();
    ImGui::NewLine();

    ImGui::BeginChild("tool_bar_draw_mode", ImVec2(24, 80));
    if(ImGui::Button("+", ImVec2(24, 24))) {
        brushMode = BRUSH_MODE_ADD;
    }
    if(ImGui::Button("-", ImVec2(24, 24))) {
        brushMode = BRUSH_MODE_SUBSTRACT;
    }
    if(ImGui::Button("=", ImVec2(24, 24))) {
        brushMode = BRUSH_MODE_PAINT;
    }
    ImGui::EndChild();
}
