#include "TimelineTile.h"

#include <Rendering/ShaderLib.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <avg/Debug.h>

#include "Editor/InputManager.h"
#include "Rendering/RenderLib.h"
#include "Rendering/TextureLib.h"

TimelineTile::TimelineTile(Scene* scene, RenderInfo renderInfo) :
scene(scene), renderInfo(renderInfo) {

}

void TimelineTile::init() {
    
}

void TimelineTile::update() {

}

void TimelineTile::draw(WindowTileInfo tileInfo) {
    assert_msg(scene != nullptr, "Cannot draw palette, scene was not initialized");
    assert_msg(scene->palette != nullptr, "Cannot draw palette, palette was not initialized");
    
    
}

void TimelineTile::terminate() {

}

void TimelineTile::resize_callback(uint32_t width, uint32_t height) {

}


void TimelineTile::refresh() {
    
}
