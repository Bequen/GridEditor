#include "_WindowTile.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <imgui/imgui.h>
#include <string>
#include <avg/Debug.h>

#include "Editor/InputManager.h"

uint32_t _WindowTile::ID = 0;

void _WindowTile::init() {
    width = 1.0f; 
    children = nullptr;
    childrenCount = 0;
    childrenBufferSize = 0;

    tileInfo = {0, 0, 1, 1};
    editors = nullptr;
    editorCount = 0;

    state = TILE_STATE_NONE;

    id = _WindowTile::ID++;
}


void _WindowTile::update(float offsetX, float offsetY, float height) {
    double x = (Input.mouseLastX + 1.0f) / 2.0f;
    double y = 1.0f - (Input.mouseLastY + 1.0f) / 2.0f;

    if(flow == TILE_FLOW_VERTICAL) {
        std::swap(x, y);
        std::swap(offsetX, offsetY);
    }

    if(width < 1.0f) {
        if(Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_PRESS) {
            if(y > width - 0.01f && y < width) {
                state = TILE_STATE_RESIZE_BEGIN;
                resizeHighestBound = resizeLowestBound = 0.0f;
            }
        } else if(Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_NONE && state == TILE_STATE_RESIZE) {
            state = TILE_STATE_RESIZE_END;
            resizeHighestBound = resizeLowestBound = 0.0f;
        }
    }

    if(childrenCount == 0) {
        
    } else {
        float offset = offsetY;

        for(uint32_t i = 0; i < childrenCount; i++) {
            if(children[i].state == TILE_STATE_RESIZE) {
                /* if(x < children[i].resizeLowestBound + 0.05f)
                    children[i].width = children[i].resizeLowestBound + 0.05f;
                else if(x > children[i].resizeHighestBound - 0.05f)
                    children[i].width = children[i].resizeHighestBound - 0.05f;
                else */
                    children[i].width = x;
            }

            children[i].update(offset, offsetX, width);

            // If the player started resizing, find the bounds
            if(children[i].state == TILE_STATE_RESIZE_BEGIN) {
                if(i < childrenCount - 1) {
                    children[i].state = TILE_STATE_RESIZE;
                    children[i].resizeLowestBound = children[i].find_highest_bound(flow, offset);
                    children[i].resizeHighestBound = children[i + 1].find_lowest_bound(flow);
                    MESSAGE("Resize bounds: " << children[i].resizeLowestBound << ":" << children[i].resizeHighestBound);
                } else {
                    children[i].state = TILE_STATE_NONE;
                }
            } else if(children[i].state == TILE_STATE_RESIZE_END) {
                children[i].resize_callback();
                children[i + 1].resize_callback();
                children[i].state = TILE_STATE_NONE;
            }

            offset = children[i].width;
        }
    }
}

void _WindowTile::draw(float offsetX, float offsetY, float height, uint32_t flow, int32_t index) {
    if(childrenCount == 0) {
        float w = width;

        if(flow == TILE_FLOW_HORIZONTAL) {
            std::swap(offsetX, offsetY);
            std::swap(height, w);
        } 
        std::string label = "tile_window##" + std::to_string(id);

        ImGui::Begin(label.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);


        float sizeOffsetX = 0.0;
//        if((uint32_t)(w * Input.windowWidth) % 2 == 0)
//            sizeOffsetX = ((index) % 2 == 0) ? 2.0 / Input.windowWidth : 0.0f;

        ImGui::SetWindowPos(ImVec2((offsetX - sizeOffsetX / 2.0) * Input.windowWidth, (offsetY) * Input.windowHeight));
        ImGui::SetWindowSize(ImVec2((w - offsetX + sizeOffsetX) * Input.windowWidth, (height - offsetY) * Input.windowHeight));

        tileInfo.x = offsetX;
        tileInfo.y = offsetY;
        tileInfo.width = w - offsetX;
        tileInfo.height = height - offsetY;

        label = "tile_tabs##" + std::to_string(id);
        if(ImGui::BeginTabBar(label.c_str(), ImGuiTabBarFlags_None)) {
            label = "tile_tab_item##" + std::to_string(id);
            if(ImGui::BeginTabItem("TestTab")) {
#if WINDOW_MANAGER_TEST
                    ImGui::Text("%f:%f = %f:%f", offsetX, offsetY, w - offsetX, height - offsetY);
                    if(state == TILE_STATE_RESIZE) {
                        ImGui::Text("Resizing");
                        ImGui::Text("Min: %f, Max: %f", resizeLowestBound, resizeHighestBound);
                    } 
#endif
                if(editorCount == 0) {
                    ImGui::Text("Select Editor");
                } else {
                    if(editors != nullptr) {
                        float offsetHeight = 26.0f;
                        offsetHeight = offsetHeight / Input.windowHeight;

                        editors->draw({ offsetX, 
                                        offsetY + offsetHeight, 
                                        w - offsetX, 
                                        height - offsetY - offsetHeight});
                    }
                }

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    } else {
        float offset = offsetY;
        for(uint32_t i = 0; i < childrenCount; i++) {
            children[i].draw(offset, offsetX, width, 1 - flow, i);
            offset = children[i].width;
        }
    }
}

void _WindowTile::insert_window(_WindowTile window, uint32_t index) {
    MESSAGE("Inserting to " << id << " at " << index);

    // If there is need to resize
    if(childrenBufferSize == childrenCount) {
        childrenBufferSize += 4;
        _WindowTile* temp = new _WindowTile[childrenBufferSize];

        memcpy(temp, children, childrenCount * sizeof(_WindowTile));
        delete [] children;

        children = temp;
    }

    window.flow = 1 - flow;


    if(childrenCount == 0) {
        // Normalizes the index
        if(index > 1)
            index = 1;

        // Assign the windows
        children[1 - index] = *this;
        children[1 - index].children = nullptr;
        children[1 - index].childrenCount = 0;
        children[1 - index].childrenBufferSize = 0;
        children[1 - index].flow = 1 - flow;
        children[index] = window;
        children[index].children = nullptr;

        WindowTileInfo info;
        info.x = tileInfo.y;
        info.y = tileInfo.x;

        info.width = tileInfo.height / 2.0f;
        info.height = tileInfo.width;

        children[0].tileInfo = info;
        children[0].width = info.x + info.width;

        children[1].tileInfo = info;
        children[1].width = info.x + info.width + info.width;
        children[1].tileInfo.x += info.width;

        MESSAGE("Widths: " << children[0].width << ":" << children[1].width << " at " << children[0].tileInfo.x << ":" << children[1].tileInfo.x);
        
        childrenCount = 2;
    } else {
        // Normalize the index
        if(index > childrenCount)
            index = childrenCount;

        float w = children[index].tileInfo.x + children[index].tileInfo.width / 2.0f;
        MESSAGE("Width: " << w);
        // Moves the array for insertion
        memmove(&children[index + 1], &children[index], (childrenCount - index) * sizeof(_WindowTile));

        WindowTileInfo info;
        info.x = tileInfo.y;
        info.y = tileInfo.x;

        if(index > 0)
            info.x = children[index - 1].width;

        info.width = w - tileInfo.x;
        info.height = children[index + 1].tileInfo.height;

        window.tileInfo = info;
        window.width = w;
        children[index] = window;

        childrenCount++;
    }
}

float _WindowTile::find_lowest_bound(uint32_t flow) {
    float result = width;
    if(children != nullptr && childrenCount > 1) {
        for(uint32_t i = 0; i < childrenCount; i++) {
            if(children[i].children != nullptr && childrenCount > 1) {
                for(uint32_t x = 0; x < children[i].childrenCount; x++) {
                    float r = children[i].children[x].find_lowest_bound(flow);
                    if(r < result) {
                        result = r;
                    }
                }
            }
        }
    }

    return result;
}

float _WindowTile::find_highest_bound(uint32_t flow, float initial) {
    float result = initial;
    for(uint32_t i = 0; i < childrenCount; i++) {
        if(children[i].childrenCount > 1) {
            float r = children[i].children[children[i].childrenCount - 1].find_highest_bound(flow, children[i].children[children[i].childrenCount - 2].width);
            if(r > result) {
                result = r;
            }
        }
    }

    return result;
}

void _WindowTile::resize_callback() {
    if(editors != nullptr)
        editors->resize_callback(Input.windowWidth, Input.windowHeight);
    
    for(uint32_t i = 0; i < childrenCount; i++) {
        children[i].resize_callback();
    }
}

void _WindowTile::refresh() {
    if(childrenCount == 0) {
        if(editors != nullptr)
            editors->refresh();
    } else {
        for(uint32_t i = 0; i < childrenCount; i++) {
            children[i].refresh();
        }
    }
}
