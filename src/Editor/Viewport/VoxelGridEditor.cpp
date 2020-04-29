#include "VoxelGridEditor.h"

#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"

#include <cstring>
#include <avg/Debug.h>
#include <Rendering/TextureLib.h>
#include <glm/gtx/intersect.hpp>

void VoxelGridEditor::assign(const SceneObject* sceneObject, Scene* scene, ViewportInfo* viewport) {
    //this->grid = (Grid*)sceneObject->data;
    this->grid = (SceneGrid*)sceneObject->data;
    this->scene = scene;
    this->viewport = viewport;

    this->tempGrid = *this->grid;
    this->tempGrid.grid.buffer = new int8_t[tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height];
    memset(this->tempGrid.grid.buffer, 0, tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height);
}

void VoxelGridEditor::init() {
    drawMode = DRAW_MODE_BRUSH;
    brushMode = BRUSH_MODE_ADD;
    isDrawing = false;
    requireUpdate = false;
}

void VoxelGridEditor::update(RenderInfo renderInfo) {
    viewport->camera.update();
    solve_voxel_drawing();

    RenderLib::draw_sky(renderInfo, viewport->camera.mode);

    RenderLib::front_face(GL_CW);
    RenderLib::bind_vertex_array(renderInfo.voxelVAO);
    RenderLib::draw_voxel(renderInfo.boxProgram, scene->selected->transform.transform, glm::vec3(tempGrid.grid.width, tempGrid.grid.depth, tempGrid.grid.height));
    RenderLib::front_face(GL_CCW);

    ShaderLib::program_use(renderInfo.voxelProgram);
    glm::vec3 camPos = viewport->camera.origin + viewport->camera.direction * -viewport->camera.offset;
    ShaderLib::uniform_vec3(renderInfo.voxelProgram, "cameraPos", &camPos.x);

    //RenderLib::draw_grid(renderInfo, _tempGrid, scene->selected->transform);
    draw_grid(renderInfo, &tempGrid);
}

void VoxelGridEditor::draw_grid(RenderInfo renderInfo, const SceneGrid* grid) {
    uint32_t streak = 0;
    uint32_t streakBegin = 0;

    glUseProgram(renderInfo.voxelProgram);
    
    glBindVertexArray(renderInfo.voxelVAO);
    glUniform3ui(glGetUniformLocation(renderInfo.voxelProgram, "size"), grid->grid.width, grid->grid.depth, grid->grid.height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, grid->voxelGrid.texture);

    

    glUseProgram(renderInfo.voxelSelectedProgram);
    uint32_t positionUniform = glGetUniformLocation(renderInfo.voxelSelectedProgram, "index");

    glUniform3ui(glGetUniformLocation(renderInfo.voxelSelectedProgram, "size"), grid->grid.width, grid->grid.depth, grid->grid.height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, grid->voxelGrid.texture);

    for(uint32_t i = 0; i < selection.selectedCount; i++) {
        glUniform1i(positionUniform, selection.selection[i]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glUseProgram(renderInfo.voxelProgram);
    positionUniform = glGetUniformLocation(renderInfo.voxelProgram, "index");

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
    }
}

void VoxelGridEditor::terminate() {
    
}


void VoxelGridEditor::resize_callback() {

}

void VoxelGridEditor::refresh_callback() {

}

void VoxelGridEditor::solve_voxel_drawing() {
    if(Input.get(GLFW_KEY_LEFT_SHIFT) == KEY_STATE_HELD)
        drawMode = DRAW_MODE_SHAPE;
    else if(Input.get(GLFW_KEY_LEFT_ALT) == KEY_STATE_HELD)
        drawMode = DRAW_MODE_EXTRUDE;
    else
        drawMode = DRAW_MODE_BRUSH;

    double cursorX, cursorY;

    Input.get_mapped_cursor(viewport->tileInfo, &cursorX, &cursorY);

    Ray ray = viewport->camera.create_ray(glm::vec3(cursorX, cursorY, 0.0f));
    RayHit hit = ray_cast(ray);

    shape.end = brushMode == BRUSH_MODE_ADD ? hit.point + hit.normal : hit.point;

    if( std::floor(shape.end[0]) != std::floor(shapeEndTemp[0]) ||
        std::floor(shape.end[1]) != std::floor(shapeEndTemp[1]) ||
        std::floor(shape.end[2]) != std::floor(shapeEndTemp[2])) { 
        
        //shape.end = shapeEndTemp;
        requireUpdate = true;
    }

    uint32_t index = get_index(shape.end);

    if(drawMode == DRAW_MODE_BRUSH) {
        if(Input.get(GLFW_MOUSE_BUTTON_1) != KEY_STATE_NONE) {
            if(isDrawing) {
                if(brushMode == BRUSH_MODE_PAINT) {
                    if(tempGrid.get(index) > 0) {
                        
                        //tempCache.buffer[tempCache.count++] = {index, selectedGrid->get(index), scene->colorSelected};
                        tempGrid.set(index, scene->colorSelected);
                    }
                } else {
                    //tempCache.buffer[tempCache.count++] = {index, selectedGrid->get(index), 0};
                    tempGrid.set(index, scene->colorSelected);
                }
                requireUpdate = true;
                //update_grid(&_tempGrid);
                tempGrid.update_texture();
            } else {
                isDrawing = true;
            }
        } else {
            if(isDrawing) {
                isDrawing = false;
                //update_cache();
                memcpy(grid->grid.buffer, tempGrid.grid.buffer, tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height);
                /* update_grid(_grid); */
                grid->update_texture();
                requireUpdate = false;
            }
        }
    } else if(drawMode == DRAW_MODE_SHAPE) {
        if(Input.get(GLFW_MOUSE_BUTTON_1) != KEY_STATE_NONE) {
            if(isDrawing) {
                memcpy(tempGrid.grid.buffer, grid->grid.buffer, tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height);

                solve_shape(&tempGrid, shape.start, shape.end);
                //update_grid(&_tempGrid);
                tempGrid.update_texture();
            } else {
                isDrawing = true;
                shape.start = shape.end;
            }
        } else {
            if(isDrawing) {
                isDrawing = false;

                solve_shape(&tempGrid, shape.start, shape.end);
                memcpy(grid->grid.buffer, tempGrid.grid.buffer, tempGrid.grid.width * tempGrid.grid.depth * tempGrid.grid.height);
                //update_grid(grid);
                grid->update_texture();
            }
        }
    } else if(drawMode == DRAW_MODE_EXTRUDE) {
        if(Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_HELD) {
            if(isDrawing) {
                /* float result = 0.0f;

                glm::vec3 planeNormal = glm::normalize(ray.origin - shape.start);
                planeNormal = planeNormal * (glm::vec3(1.0f, 1.0f, 1.0f) - glm::normalize(shape.normal));
                glm::intersectRayPlane(ray.origin, ray.direction, shape.start, glm::normalize(planeNormal), result);
                glm::vec3 r = (ray.origin + ray.direction * result);

                memcpy(tempGrid.grid.buffer, grid->grid.buffer, tempGrid.width() * tempGrid.depth() * tempGrid.height());
                float height = glm::distance(r * shape.normal, shape.start * shape.normal);
                if(glm::dot(shape.normal, (r - shape.start)) < 0.0f)
                    height *= -1.0f;

                extrude(height);

                //update_grid(&tempGrid);
                tempGrid.update_texture(); */
            } else {
                if(selection.selection != nullptr)
                    delete [] selection.selection;

                shape.start = shape.end - hit.normal;
                selection.selection = new uint32_t[tempGrid.width() * tempGrid.depth() * tempGrid.height()];
                memset(selection.selection, 0, sizeof(uint32_t) * (tempGrid.width() * tempGrid.depth() * tempGrid.height()));
                selection.selectedCount = 0;
                
                MESSAGE("Selecting on " << shape.start.x << ":" << shape.start.y << ":" << shape.start.z);
                ERROR("Normal: " << hit.normal.x << ":" << hit.normal.y << ":" << hit.normal.z);
                flood_fill(floor_vec(shape.start), hit.normal);
                MESSAGE("Selected " << selection.selectedCount);
                for(uint32_t i = 0; i < selection.selectedCount; i++)
                    WARNING(selection.selection[i]);
                shape.normal = hit.normal;
                isDrawing = true;
            }
        } else {
            if(isDrawing) {
                isDrawing = false;
                memcpy(grid->grid.buffer, tempGrid.grid.buffer, tempGrid.width() * tempGrid.depth() * tempGrid.height());
                //update_grid(grid);
                grid->update_texture();
                //selection.selectedCount = 0;
            }
        }
    }
}

void VoxelGridEditor::extrude(int32_t height) {
    //WARNING(height << " " << selection.selectedCount);
    int32_t shapeNormalOffset = get_index(shape.normal);
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

void VoxelGridEditor::solve_shape(SceneGrid* grid, glm::vec3 start, glm::vec3 end) {
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
    return std::floor(position.x) + std::floor(position.y) * tempGrid.grid.width + std::floor(position.z) * (tempGrid.grid.depth * tempGrid.grid.width);
}

void VoxelGridEditor::update_grid(const Grid* grid) {
    TextureLib::update_texture_3d(grid->gridTexture, grid->width, grid->depth, grid->height, grid->buffer);
}

void VoxelGridEditor::flood_fill(glm::vec3 position, glm::vec3 normal) {
    if(position.x < 0 || position.x > tempGrid.grid.width ||
        position.y  < 0 || position.y > tempGrid.grid.depth ||
        position.z < 0 || position.z > tempGrid.grid.height) {
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
        uint32_t index = get_index(position);
        for(uint32_t i = 0; i < selection.selectedCount; i++)
            if(selection.selection[i] == index)
                return;
        selection.selection[selection.selectedCount++] = index;

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