#include "SpriteEditor.h"

#include <cstring>
#include <avg/Debug.h>

#include "Rendering/TextureLib.h"
#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"

void SpriteEditor::assign(const SceneObject* sceneObject, Scene* scene, ViewportInfo* viewport) {
    this->sprite = (SceneSprite*)sceneObject->data;
    this->scene = scene;
    this->viewport = viewport;

    this->tempSprite = *this->sprite;
    this->tempSprite.grid.buffer = new char[tempSprite.grid.width * tempSprite.grid.height];
    memset(this->tempSprite.grid.buffer, 0, tempSprite.grid.width * tempSprite.grid.height);
}

void SpriteEditor::init() {
    drawMode = DRAW_MODE_BRUSH;
    brushMode = BRUSH_MODE_ADD;
    isDrawing = false;
    requireUpdate = false;
}

void SpriteEditor::update(RenderInfo renderInfo) {
    viewport->camera.update();
    solve_painting();

    RenderLib::draw_sky(renderInfo, viewport->camera.mode);

    RenderLib::front_face(GL_CW);
    RenderLib::bind_vertex_array(renderInfo.voxelVAO);
    RenderLib::draw_voxel(renderInfo.boxProgram, scene->selected->transform.transform, glm::vec3(tempSprite.grid.width, 1.0f, tempSprite.grid.height));
    RenderLib::front_face(GL_CCW);

    ShaderLib::program_use(renderInfo.voxelProgram);
    glm::vec3 camPos = viewport->camera.origin + viewport->camera.direction * -viewport->camera.offset;
    ShaderLib::uniform_vec3(renderInfo.voxelProgram, "cameraPos", &camPos.x);

    draw_sprite(renderInfo, &tempSprite);
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
                ERROR("Setting on " << ray.origin.x << ":" << ray.origin.z << " the " << (int)scene->colorSelected);
                tempSprite.set(glm::vec2(ray.origin.x, ray.origin.z), scene->colorSelected);

                tempSprite.update_texture();
            } else {
                isDrawing = true;
            }
        } else {
            if(isDrawing) {
                isDrawing = false;
                memcpy(sprite->grid.buffer, tempSprite.grid.buffer, tempSprite.width() * tempSprite.height());
            }
        }
    }
}

void SpriteEditor::draw_sprite(RenderInfo renderInfo, const SceneSprite* sprite) {
    uint32_t streak = 0;
    uint32_t streakBegin = 0;

    glUseProgram(renderInfo.spriteProgram);
    
    glBindVertexArray(renderInfo.voxelVAO);
    glUniform3ui(glGetUniformLocation(renderInfo.spriteProgram, "size"), sprite->grid.width, 1.0f, sprite->grid.height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite->sprite.texture);

    

    /* glUseProgram(renderInfo.voxelSelectedProgram);
    uint32_t positionUniform = glGetUniformLocation(renderInfo.voxelSelectedProgram, "index"); */

    /* glUniform3ui(glGetUniformLocation(renderInfo.voxelSelectedProgram, "size"), sprite->grid.width, 1.0f, sprite->grid.height);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sprite->sprite.texture); */
    glUseProgram(renderInfo.spriteProgram);
    uint32_t positionUniform = glGetUniformLocation(renderInfo.spriteProgram, "index");

   /*  for(uint32_t i = 0; i < selection.selectedCount; i++) {
        if(tempSprite.get(selection.selection[i]) > 0) {
            glUniform1i(positionUniform, selection.selection[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
    } */

    
    positionUniform = glGetUniformLocation(renderInfo.spriteProgram, "index");

    for(uint32_t i = 0; i < tempSprite.grid.width * tempSprite.grid.height; i++) {
        if(tempSprite.get(i) > 0) {
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

void SpriteEditor::terminate() {
    
}


void SpriteEditor::resize_callback() {

}

void SpriteEditor::refresh_callback() {

}