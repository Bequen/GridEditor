#include "SceneExplorerTile.h"

#include <Rendering/ShaderLib.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
#include <avg/Debug.h>
#include <Rendering/RenderLib.h>

#include "Rendering/TextureLib.h"

SceneExplorerTile::SceneExplorerTile(Scene* scene, RenderInfo renderInfo) :
scene(scene), renderInfo(renderInfo) {

}

void SceneExplorerTile::init() {
    
}

void SceneExplorerTile::update() {
    
    
}

void SceneExplorerTile::draw(WindowTileInfo tileInfo) {
    ImGui::BeginChild("scene_explorer");

    tree_node(&scene->sceneGraph);

    ImGui::EndChild();
}

void SceneExplorerTile::terminate() {

}

void SceneExplorerTile::resize_callback(uint32_t width, uint32_t height) {

}


void SceneExplorerTile::refresh() {
    
}

void SceneExplorerTile::tree_node(SceneObject* sceneObject) {
    if(sceneObject->childrenCount > 0) {
        bool open = ImGui::TreeNode("scene_graph%s%i", sceneObject->name, sceneObject->id);
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Add Grid")) {
                sceneObject->add_child(SceneObject("new grid", OBJECT_TYPE_GRID, scene->add_grid(SceneGrid(32))));
            } if (ImGui::MenuItem("Add Light")) {
                Light light = Light();
                light.direction = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                light.ambient = glm::vec4(23.47f, 21.31f, 20.79f, 1.0f);

                sceneObject->add_child(SceneObject(OBJECT_TYPE_LIGHT, scene->add_light(light)));
                update_lights();
            } if (ImGui::MenuItem("Add Sprite")) {
                sceneObject->add_child(SceneObject(OBJECT_TYPE_SPRITE, scene->add_sprite(SceneSprite(32, 32))));
            } if (ImGui::MenuItem("Delete")) {
                WARNING("deleting");
                scene->selected = nullptr;
                scene->sceneGraph.remove(sceneObject);
            }
            ImGui::EndPopup();
        }
        if (open) {
            for(uint32_t i = 0; i < sceneObject->childrenCount; i++) {
                tree_node(&sceneObject->children[i]);
            }
            ImGui::TreePop();
        }
    } else {
        std::string str_id = "tree_node##" + std::to_string(sceneObject->id);
        ImGui::BulletText(sceneObject->name);
        if(ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
            scene->selected = sceneObject;
            ERROR("Selected " << sceneObject->id << " of type " << sceneObject->type);
        }

        if (ImGui::BeginPopupContextItem("Item context menu")) {
            if (ImGui::MenuItem("Add Grid")) {
                sceneObject->add_child(SceneObject("new grid", OBJECT_TYPE_GRID, scene->add_grid(SceneGrid(32))));
            } if (ImGui::MenuItem("Add Light")) {
                Light light = Light();
                light.direction = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                light.ambient = glm::vec4(23.47f, 21.31f, 20.79f, 1.0f);

                sceneObject->add_child(SceneObject(OBJECT_TYPE_LIGHT, scene->add_light(light)));
                update_lights();
            } if (ImGui::MenuItem("Add Sprite")) {
                sceneObject->add_child(SceneObject(OBJECT_TYPE_SPRITE, scene->add_sprite(SceneSprite(32, 32))));
            } if (ImGui::MenuItem("Delete")) {
                if(!scene->isEditMode) {
                    WARNING("Deleting");
                    if(scene->selected == sceneObject) {
                        scene->selected = nullptr;
                    }
                    scene->sceneGraph.remove(sceneObject);
                } else {
                    if (ImGui::BeginPopup("Options"))
                    {
                        ImGui::Text("You cannot remove item when in edit mode");
                        ImGui::EndPopup();
                    }
                }
            }
            ImGui::EndPopup();
        }
    }
}

void SceneExplorerTile::update_lights() {
    MESSAGE("Updating lights");
    void* pointer = RenderLib::map_buffer_range(scene->lightBuffer, GL_UNIFORM_BUFFER, 0, sizeof(Light) * MAX_LIGHT_COUNT);
    memcpy(pointer, scene->lights, sizeof(Light) * MAX_LIGHT_COUNT);

    RenderLib::unmap_buffer(GL_UNIFORM_BUFFER); 
}
