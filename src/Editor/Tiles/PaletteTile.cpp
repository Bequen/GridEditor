#include "PaletteTile.h"

#include <Rendering/ShaderLib.h>
#include <glad/glad.h>
#include <ImGui/imgui.h>
#include <avg/Debug.h>

#include "Editor/InputManager.h"
#include "Rendering/RenderLib.h"
#include "Rendering/TextureLib.h"

PaletteTile::PaletteTile(Scene* scene, RenderInfo renderInfo) :
scene(scene), renderInfo(renderInfo) {

}

void PaletteTile::init() {
    scene->colorSelected = 1;
}

void PaletteTile::update() {

}

void PaletteTile::draw(WindowTileInfo tileInfo) {
    assert_msg(scene != nullptr, "Cannot draw palette, scene was not initialized");
    assert_msg(scene->palette != nullptr, "Cannot draw palette, palette was not initialized");
    ImGui::BeginChild("PickerWindow", ImVec2(200.0f, 200.0f), true);

    /* if(ImGui::ColorPicker3("picker", &scene->palette[scene->colorSelected].r))
        update_palette(); */
    if(ImGui::ColorPicker3("picker", &scene->materials[scene->colorSelected].albedo.r))
        update_palette(); 
    if(ImGui::SliderFloat("Metalness", &scene->materials[scene->colorSelected].brdf.r, 0.0f, 1.0f))
        update_palette();
    if(ImGui::SliderFloat("Roughness", &scene->materials[scene->colorSelected].brdf.g, 0.0f, 1.0f))
        update_palette();

    ImGui::EndChild();

    ImGui::BeginChild("PaletteWindow", ImVec2(0.0f, 0.0f), true);
    float buttonSize = 20.0f;
    float padding = 5.0f;

    float size = ImGui::GetWindowContentRegionWidth();
    int rowSize = (int)std::floor(size / (buttonSize + padding));
    for(uint32_t i = 0; i < 256; i++) {
        const char* label = std::to_string(i).c_str();
        if(rowSize > 0) {
            if(i % rowSize)
                ImGui::SameLine((i % rowSize) * (buttonSize + padding) + padding * 2);
            if(ImGui::ColorButton(label, ImVec4(scene->materials[i].albedo.x, scene->materials[i].albedo.y, scene->materials[i].albedo.z, 1.0f), 0, ImVec2(buttonSize, buttonSize))) {
                scene->colorSelected = i;
                scene->colorCache = i;
            }
        }
    }
    
    ImGui::EndChild();
}

void PaletteTile::terminate() {

}

void PaletteTile::update_palette() {
    TextureLib::update_texture_1d(scene->paletteTexture, 256, scene->palette);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_1D, scene->paletteTexture);

    MESSAGE("Updating materials");
    void* pointer = RenderLib::map_buffer_range(scene->materialsBuffer, GL_UNIFORM_BUFFER, 0, sizeof(Light) * MAX_LIGHT_COUNT);
    memcpy(pointer, scene->materials, sizeof(Light) * MAX_LIGHT_COUNT);

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER); 
}

void PaletteTile::resize_callback(uint32_t width, uint32_t height) {

}


void PaletteTile::refresh() {
    update_palette();
}