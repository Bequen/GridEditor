#include "PaletteTile.h"

#include <Rendering/ShaderLib.h>
#include <glad/glad.h>
#include <ImGUI/imgui.h>

void PaletteTile::init() {

}

void PaletteTile::update() {

}

void PaletteTile::draw(Cursor cursor, WindowTileInfo tileInfo) {
    ImGui::BeginChild("PickerWindow", ImVec2(200.0f, 200.0f), true);
    // Stupid workaround
    if(scene->colorSelected > 255)
        scene->colorSelected = 1;
    if(ImGui::ColorPicker3("picker", &scene->palette[scene->colorSelected].r, ImGuiColorEditFlags_PickerHueWheel))
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
            if(ImGui::ColorButton(label, ImVec4(scene->palette[i].r, scene->palette[i].g, scene->palette[i].b, 1.0f), 0, ImVec2(buttonSize, buttonSize))) {
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
/*     ShaderLib::uniform_int32(render.shader, "palette", 1); */
}

void PaletteTile::resize_callback(uint32_t width, uint32_t height) {

}