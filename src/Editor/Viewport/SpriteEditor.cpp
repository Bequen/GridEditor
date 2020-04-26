#include "SpriteEditor.h"

#include <cstring>
#include <avg/Debug.h>

#include "Rendering/TextureLib.h"
#include "Rendering/RenderLib.h"

void SpriteEditor::assign(const SceneObject* sceneObject, Scene* scene, ViewportInfo* viewport) {
    this->scene = scene;
    this->viewport = viewport;

    this->grid = (Grid*)sceneObject->data;

    this->tempGrid = *this->grid;
    this->tempGrid.buffer = new int8_t[tempGrid.width * tempGrid.depth * tempGrid.height];
    memset(this->tempGrid.buffer, 0, tempGrid.width * tempGrid.depth * tempGrid.height);
}

void SpriteEditor::init() {
    this->viewport->camera.set_mode(CAMERA_MODE_ORTHOGRAPHIC);
    this->viewport->camera.direction = glm::vec3(0.0f, 1.0f, 0.0f);

    drawMode = DRAW_MODE_BRUSH;
    brushMode = BRUSH_MODE_ADD;
    isDrawing = false;
    requireUpdate = false;
}

void SpriteEditor::update(RenderInfo renderInfo) {
    solve_painting();

    RenderLib::draw_sky(renderInfo, viewport->camera.mode);

    RenderLib::front_face(GL_CW);
    RenderLib::bind_vertex_array(renderInfo.voxelVAO);
    RenderLib::draw_voxel(renderInfo.boxProgram, scene->selected->transform.transform, glm::vec3(tempGrid.width, tempGrid.depth, tempGrid.height));
    RenderLib::front_face(GL_CCW);

    Transform transform;
    transform.transform = glm::mat4(1.0f);
    transform.transform = glm::scale(transform.transform, glm::vec3(32.0f, 0.0f, 32.0f));
    RenderLib::draw_sprite(renderInfo, grid->gridTexture, transform);
}

void SpriteEditor::solve_painting() {
    double cursorX, cursorY;
    Input.get_mapped_cursor(viewport->tileInfo, &cursorX, &cursorY);

    Ray ray = viewport->camera.create_ray(glm::vec3(cursorX, cursorY, 1.0f));
    
    if(Input.get(GLFW_KEY_LEFT_SHIFT))
        drawMode = DRAW_MODE_SHAPE;
    else if(Input.get(GLFW_KEY_LEFT_ALT))
        drawMode = DRAW_MODE_EXTRUDE;
    else
        drawMode = DRAW_MODE_BRUSH;

    if(drawMode == DRAW_MODE_BRUSH) {
        if(Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_HELD) {
            if(isDrawing) {
                ERROR("Setting on " << ray.origin.x << ":" << ray.origin.z);
                tempGrid.set(glm::vec3(ray.origin.x, 0.0f, ray.origin.z), scene->colorSelected);

                //update_grid(&tempGrid);
                TextureLib::update_sub_texture_2d(grid->gridTexture, (uint32_t)std::floor(ray.origin.x), (uint32_t)std::floor(ray.origin.x), 1, 1, &scene->palette[scene->colorSelected]);
            } else {
                isDrawing = true;
            }
        } else {
            if(isDrawing) {
                isDrawing = false;
                memcpy(grid->buffer, tempGrid.buffer, tempGrid.width * tempGrid.height);
            }
        }
    }
}

void SpriteEditor::terminate() {
    
}


void SpriteEditor::resize_callback() {

}

void SpriteEditor::refresh_callback() {

}

void SpriteEditor::update_grid(const Grid* grid) {
    //TextureLib::update_texture_2d(grid->gridTexture, grid->width, grid->depth, grid->buffer);
}