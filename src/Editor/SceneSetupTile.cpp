#include "SceneSetupTile.h"

#include "Rendering/RenderLib.h"
#include <ImGUI/imgui.h>
#include <glad/glad.h>

void SceneSetupTile::init() {

}

void SceneSetupTile::update() {
    
}

void SceneSetupTile::terminate() {
    
}

void SceneSetupTile::draw(Cursor cursor, WindowTileInfo tileInfo) {
    // All the lights
    ImGui::BeginChild("Scene Lights");
    for(uint32_t i = 0; i < scene->lightCount; i++) {
        char* buffer = new char[256];
        sprintf(buffer, "LightLabel%i", i);

        ImGui::BeginChild(buffer, ImVec2(0.0f, 100.0f), true, ImGuiWindowFlags_NoMove);
        ImGui::Text("Light %i", i);
        if(ImGui::InputFloat3("Position", &scene->lights[i].position.x, 3))
            update_lights();
        if(ImGui::InputFloat3("Direction", &scene->lights[i].direction.x, 3))
            update_lights();

        ImGui::EndChild();

        delete [] buffer;
    }

    // Button for adding new light
    if(scene->lightCount < scene->lightBufferSize) {
        ImGui::BeginChild("AddLightLabel", ImVec2(200.0f, 0.0f));
        if(ImGui::Button("AddLightButton")) {
            scene->lightCount++;
        }
        ImGui::EndChild();
    }

    ImGui::EndChild();

}

void SceneSetupTile::resize_callback(uint32_t width, uint32_t height) {
    
}

void SceneSetupTile::update_lights() {
    void* pointer = RenderLib::map_buffer_range(scene->lightBuffer, GL_UNIFORM_BUFFER, 0, sizeof(Light) * scene->lightBufferSize);
    memcpy(pointer, scene->lights, sizeof(Light) * MAX_LIGHT_COUNT);

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER); 
}