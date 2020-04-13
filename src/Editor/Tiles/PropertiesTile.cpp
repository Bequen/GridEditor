#include "PropertiesTile.h"

#include <Rendering/ShaderLib.h>
#include <glad/glad.h>
#include <ImGui/imgui.h>
#include <avg/StringLib.h>
#include <avg/Debug.h>

#include "Rendering/TextureLib.h"

PropertiesTile::PropertiesTile(Scene* scene, RenderInfo renderInfo) :
scene(scene), renderInfo(renderInfo) {

}

void PropertiesTile::init() {
    scene->colorSelected = 1;
}

void PropertiesTile::update() {

}

void PropertiesTile::draw(WindowTileInfo tileInfo) {
    assert_msg(scene != nullptr, "Cannot draw palette, scene was not initialized");
    assert_msg(scene->palette != nullptr, "Cannot draw palette, palette was not initialized");

    switch(scene->selected->type) {
        case OBJECT_TYPE_GRID: {
            ImGui::Text("Grid");
            Grid grid;
            memcpy(&grid, scene->selected->data, sizeof(Grid));

            float size[] = {grid.width, grid.depth, grid.height};
            char* text = new char[256];
            memset(text, 0, 256);

            sprintf(text, "%i %i %i", grid.width, grid.depth, grid.height);

            if(ImGui::InputText("size", text, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
                uint32_t splitCount = 0;
                char** splits = avg::StringLib::split(text, ' ', &splitCount);

                if(splitCount == 0)
                    break;
                else if(splitCount == 1) {
                    ((Grid*)scene->selected->data)->resize(std::atoi(splits[0]), std::atoi(splits[0]), std::atoi(splits[0]));
                } else if(splitCount == 3) {
                    ((Grid*)scene->selected->data)->resize(std::atoi(splits[0]), std::atoi(splits[1]), std::atoi(splits[2]));
                }
            }
            ImGui::IsItemFocused();
            break;
        } case OBJECT_TYPE_LIGHT: {
            ImGui::Text("Light");
            Light* light = (Light*)scene->selected->data;

            float direction[] = {light->direction.x, light->direction.y, light->direction.z, light->direction.w};
            float position[] = {light->position.x, light->position.y, light->position.z, light->position.w};
            float ambient[] = {light->ambient.x, light->ambient.y, light->ambient.z, light->ambient.w};

            if(ImGui::InputFloat4("Direction", direction)) {
                light->direction = glm::vec4(direction[0], direction[1], direction[2], direction[3]);
            } if(ImGui::InputFloat4("Position", position)) {
                light->position = glm::vec4(position[0], position[1], position[2], position[3]);
            } if(ImGui::InputFloat4("Ambient", ambient)) {
                light->ambient = glm::vec4(ambient[0], ambient[1], ambient[2], ambient[3]);
            }

            scene->update_lights();
            break;
        }
    }
}

void PropertiesTile::terminate() {

}

void PropertiesTile::resize_callback(uint32_t width, uint32_t height) {

}


void PropertiesTile::refresh() {
    
}