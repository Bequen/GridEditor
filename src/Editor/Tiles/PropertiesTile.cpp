#include "PropertiesTile.h"

#include <Rendering/ShaderLib.h>
#include <glad/glad.h>
#include <imgui/imgui.h>
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

    if(scene->selected != nullptr) {
        if(ImGui::InputText("Name", scene->selected->name, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {

        }

        switch(scene->selected->type) {
            case OBJECT_TYPE_GRID: 
            {
                if(scene->selected->data == nullptr) {
                    ERROR("Data is nullptr");
                    break;
                }
                ImGui::Text("Grid");
                
                SceneGrid* grid = (SceneGrid*)scene->selected->data;

                float size[] = {(float)grid->width, (float)grid->depth, (float)grid->height};
                char* text = new char[256];
                memset(text, 0, 256);

                sprintf(text, "%i %i %i", grid->width, grid->depth, grid->height);

                if(ImGui::InputText("size", text, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    uint32_t splitCount = 0;
                    char** splits = avg::StringLib::split(text, ' ', &splitCount);

                    if(splitCount == 0)
                        break;
                    else if(splitCount == 1) {
                        //((Grid*)scene->selected->data)->resize(std::atoi(splits[0]), std::atoi(splits[0]), std::atoi(splits[0]));
                        grid->resize(std::atoi(splits[0]), std::atoi(splits[0]), std::atoi(splits[0]));
                    } else if(splitCount == 3) {
                        //((Grid*)scene->selected->data)->resize(std::atoi(splits[0]), std::atoi(splits[1]), std::atoi(splits[2]));
                        grid->resize(std::atoi(splits[0]), std::atoi(splits[1]), std::atoi(splits[2]));
                    }
                }
                ImGui::IsItemFocused();
                break;
            } 
            case OBJECT_TYPE_LIGHT: 
            {
                ImGui::Text("Light");
                Light* light = (Light*)scene->selected->data;

                float direction[] = {light->direction.x, light->direction.y, light->direction.z, light->direction.w};
                float position[] = {light->position.x, light->position.y, light->position.z, light->position.w};
                float ambient[] = {light->ambient.x, light->ambient.y, light->ambient.z, light->ambient.w};

                if(ImGui::InputFloat3("Direction", direction)) {
                    light->direction = glm::vec4(direction[0], direction[1], direction[2], direction[3]);
                } if(ImGui::InputFloat3("Position", position)) {
                    light->position = glm::vec4(position[0], position[1], position[2], position[3]);
                } if(ImGui::ColorEdit3("Ambient", ambient)) {
                    light->ambient = glm::vec4(ambient[0], ambient[1], ambient[2], ambient[3]);
                } 
ImGui::SliderFloat("light_strength", &light->ambient.w, 0.0, 100.0);

                const char* items[] = { "Directional", "Point"};
                static const char* current_item = items[(uint32_t)light->position.w];

                if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
                {
                    for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                    {
                        bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
                        if (ImGui::Selectable(items[n], is_selected))
                            light->position.w = (float)n;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                    }
                    ImGui::EndCombo();
                }

                scene->update_lights();
                break;
            }
            case OBJECT_TYPE_SPRITE:
            {
                if(scene->selected->data == nullptr) {
                    ERROR("Data is nullptr");
                    break;
                }

                ImGui::Text("Sprite");

                SceneSprite* sprite = (SceneSprite*)scene->selected->data;

                char* text = new char[256];
                memset(text, 0, 256);

                sprintf(text, "%i %i", sprite->width, sprite->height);

                if(ImGui::InputText("size", text, 256, ImGuiInputTextFlags_EnterReturnsTrue)) {
                    uint32_t splitCount = 0;
                    char** splits = avg::StringLib::split(text, ' ', &splitCount);

                    if(splitCount == 0)
                        break;
                    else if(splitCount == 1) {
                        sprite->resize(std::atoi(splits[0]), std::atoi(splits[0]));
                    } else if(splitCount == 2) {
                        sprite->resize(std::atoi(splits[0]), std::atoi(splits[1]));
                    }
                }

                ImGui::BeginChild("sprite_animation_box");
                    ImGui::ListBoxHeader("Sprite Animations");

                    #pragma region Animations {
                        for(uint32_t i = 0; i < sprite->animationsCount; i++) {
                            //WARNING(sprite->currentAnimation << " " << i);
                            if(ImGui::Selectable(text, i == sprite->currentAnimation, ImGuiSelectableFlags_AllowDoubleClick)) {
                                MESSAGE("Selected " << i << ". animation");
                                sprite->change_animation(i);
                            }
                        }

                        ImGui::ListBoxFooter();
                        if(ImGui::Button("Add Animation")) {
                            sprite->add_animation("Animation");
                        }
                    #pragma endregion }

                    #pragma region Animation Frames {
                        if(sprite->animations) {
                            ImGui::ListBoxHeader("Sprite Animation Frames");

                            for(uint32_t i = 0; i < sprite->animations[sprite->currentAnimation].frameCount; i++) {
                                char* text = new char[1024];
                                sprintf(text, "Frame %i-%f\0", i, sprite->animations[sprite->currentAnimation].frames[i].frameLength);
            
                                if(ImGui::Selectable(text, (sprite->currentAnimation == i), ImGuiSelectableFlags_AllowDoubleClick)) {
                                    MESSAGE("Selected " << i << ". frame");
                                    sprite->animations[sprite->currentAnimation].currentFrame = i;
                                }
                            }

                            ImGui::ListBoxFooter();
                            if(ImGui::Button("Add Frame")) {
                                sprite->animations[sprite->currentAnimation].add_frame(sprite->width, sprite->height);
                            }
                        }
                    #pragma endregion } 
                ImGui::EndChild();

                break;
            } default: {
                ImGui::Text("Nothing selected");
            }
        }
    }
}

void PropertiesTile::terminate() {

}

void PropertiesTile::resize_callback(uint32_t width, uint32_t height) {

}


void PropertiesTile::refresh() {
    
}
