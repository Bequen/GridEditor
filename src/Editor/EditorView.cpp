#include "EditorView.h"

#include <cstdlib>
#include <avg/Debug.h>
#include <cstring>
#include <cstdio>

#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"
#include "Viewport.h"
#include <ImGUI/imgui.h>

uint32_t EditorView::lastId = 0;

void EditorView::init() {
    id = EditorView::lastId++;
    childrenBufferSize = 0;
    children = nullptr;

    tile = nullptr;
    childrenCount = 0;
}

void EditorView::init(uint32_t bufferSize) {
    id = EditorView::lastId++;
    childrenBufferSize = bufferSize;
    children = new EditorView[bufferSize];

    tile = nullptr;
    childrenCount = 0;
}

uint32_t EditorView::update(Cursor cursor, Keyboard keyboard, float offsetX, float offsetY, float height, uint32_t program, uint32_t flow, uint32_t winWidth, uint32_t winHeight) {
    uint32_t result = EDITOR_WINDOW_STATE_NONE;
    
    float x = offsetX;
    float y = offsetY;

    tileInfo.x = x;
    tileInfo.y = y;

    float cursorX = cursor.cursorX / winWidth;
    float cursorY = (cursor.cursorY - 19.0f) / winHeight;
    if(flow == EDITOR_WINDOW_FLOW_Y)
        std::swap(cursorX, cursorY);

    //ERROR(cursorX << "|" << cursorY);

    // If the user pressed the mouse meaning he wants to interact
    // The exception is interacting with the first window, can't resize that one, obviously
    if(cursor.leftButtonState == STATE_PRESS && width < 1.0f) {
        ERROR("Test " << cursorY << ":" << (tileInfo.width + tileInfo.x) << "|" << ((tileInfo.height + tileInfo.y) - 0.1f));
        if(cursorY < tileInfo.width + tileInfo.x && cursorY > (tileInfo.width + tileInfo.x) - 0.1f) {
            state = EDITOR_WINDOW_STATE_RESIZE;
            ERROR("Resizing");
        }
    } else if(cursor.leftButtonState == STATE_NONE) {
        state = EDITOR_WINDOW_STATE_NONE;
    }
    // Now let's run through the children and update those
    {
        float offset = offsetY;
        float o = 0.0f;
        // Update each one
        for(uint32_t i = 0; i < childrenCount; i++) {
            assert_msg(children, "Children are NOT initialized");
            // Move the window to the cursor position
            
            if(children[i].state == EDITOR_WINDOW_STATE_RESIZE) {
                /* if(cursorX / tileInfo.height > children[i + 1].width - 0.1f)
                    children[i].width = children[i + 1].tileInfo.width + children[i + 1].tileInfo.x - 0.1f;
                else if(cursorX / tileInfo.height < offset + 0.1f)
                    children[i].width = offset + 0.1f;
                else */
                    children[i].width = cursorX;

                //children[i + 1].resize_callback((children[i + 1].width) * winWidth, width * winHeight);
                //children[i].resize_callback((children[i].width) * winWidth, width * winHeight);
                ERROR("Resizing on " << cursorX << " to " << children[i].width);
            }

            // There is no way they can be in same direction as parent, so switching the x & y turns the direction
            uint32_t r = children[i].update(cursor, keyboard, offset, offsetX, tileInfo.width, program, 1 - flow, winWidth, winHeight);
            if(r == EDITOR_WINDOW_STATE_SPLIT)
                result = EDITOR_WINDOW_STATE_SPLIT;

            //ERROR((children[i].width));
            children[i].tileInfo.width = (children[i].width - o) * (tileInfo.height);
            children[i].tileInfo.height = tileInfo.width;
            offset += children[i].tileInfo.width;
            o = children[i].width;
        }
    }

    if(keyboard.leftControl == KEY_STATE_HOLD && cursor.leftButtonState == MOUSE_BUTTON_STATE_PRESS && result != EDITOR_WINDOW_STATE_SPLIT) {
        if(cursorY > x && cursorY < tileInfo.x + tileInfo.width && cursorX > y && cursorX < tileInfo.y + tileInfo.height) {
            result = EDITOR_WINDOW_STATE_SPLIT;
            ERROR("Splitting " << x << "|" << y << ":" << width << "|" << height);

            if(childrenBufferSize < 2) {
                init(10);
            }

            children[0].init();
            children[0].width = 0.5f;
            children[1].init();
            children[1].width = 1.0f;
            childrenCount = 2;
            ERROR("Created " << children[0].width << " & " << children[1].width);
        }
    }

    // Render the quad
    if(childrenCount == 0) {
        float w = width;
        float tileX = tileInfo.width;
        float tileY = tileInfo.height;
        // Flip the values if the flow is flipped
        if(flow == EDITOR_WINDOW_FLOW_X) {
            std::swap(x, y);
            std::swap(height, w);
            std::swap(tileX, tileY);
        } else {
           
        }
        //ERROR("Rendering on : " << x << "|" << y << " with: " << tileX << "|" << tileY);
        glm::vec4 transform = glm::vec4(x, y, w, height);
        ShaderLib::uniform_vec4(program, "transform", &transform.x);
        glm::vec4 color = glm::vec4(0.5f, x, y, 1.0f);
        ShaderLib::uniform_vec4(program, "color", &color.x);

        RenderLib::draw_triangle_strip(4);

    /*         tileInfo.height = height;
        tileInfo.width = w; */

        std::string label = std::to_string(id);
        ImGui::Begin(label.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

        ImGui::SetWindowPos(ImVec2(x * winWidth, (y) * winHeight + 19));
        ImGui::SetWindowSize(ImVec2((tileX) * winWidth, (tileY) * winHeight));

        /* if(tile != nullptr)
            tile->draw(cursor, tileInfo); */

        ImGui::End();
    }

    return result;
}

uint32_t EditorView::update_test(Cursor cursor, Keyboard keyboard, float offsetX, float offsetY, float height, uint32_t program, uint32_t flow, uint32_t winWidth, uint32_t winHeight) {
    uint32_t result = EDITOR_WINDOW_STATE_NONE;
    
    float x = offsetX;
    float y = offsetY;

    float cursorX = cursor.cursorX / winWidth;
    float cursorY = (cursor.cursorY - 19.0f) / winHeight;
    if(flow == EDITOR_WINDOW_FLOW_Y)
        std::swap(cursorX, cursorY);

    tileInfo.x = x;
    tileInfo.y = y;
    tileInfo.width = width - x;
    tileInfo.height = height - y;

    // If the user pressed the mouse meaning he wants to interact
    // The exception is interacting with the first window, can't resize that one, obviously
    if(cursor.leftButtonState == STATE_PRESS && width < 1.0f) {
        if(cursorY < width && cursorY > width - 0.1f) {
            state = EDITOR_WINDOW_STATE_RESIZE;
            result = EDITOR_WINDOW_STATE_RESIZE;

            ERROR("Resizing");
        }
    } else if(cursor.leftButtonState == STATE_NONE) {
        state = EDITOR_WINDOW_STATE_NONE;
        resize_callback(winWidth, winHeight);
    }


    // Now let's run through the children and update those
    if(childrenCount > 0) {
        float offset = offsetY;

        crossLowestBound = height;
        crossHighestBound = y;

        highestBound = children[childrenCount - 1].crossHighestBound;
        lowestBound = children[0].crossLowestBound;

        for(uint32_t i = 0; i < childrenCount; i++) {
            assert_msg(children, "Children are NOT initialized");
            // Move the window to the cursor position
            
            if(children[i].state == EDITOR_WINDOW_STATE_RESIZE) {
                if(cursorX > children[i].resizeHighestBound - 0.1f)
                    children[i].width = children[i].resizeHighestBound - 0.1f;
                else if(cursorX < children[i].resizeLowestBound + 0.1f)
                    children[i].width = children[i].resizeLowestBound + 0.1f;
                else
                    children[i].width = cursorX;
            }

            // There is no way they can be in same direction as parent, so switching the x & y turns the direction
            uint32_t r = children[i].update_test(cursor, keyboard, offset, x, width, program, 1 - flow, winWidth, winHeight);
            // Workaround, because if upcoming child won't be in split state, it would overrite
            if(r == EDITOR_WINDOW_STATE_SPLIT)
                result = EDITOR_WINDOW_STATE_SPLIT;
            if(r == EDITOR_WINDOW_STATE_RESIZE) {
                children[i].resizeHighestBound = children[i + 1].lowestBound;
                children[i].resizeLowestBound = children[i].highestBound;

                children[i].resizeLowestBound = children[i].find_bound_highest(flow, offset);
                children[i].resizeHighestBound = children[i + 1].find_bound_lowest(flow);
                ERROR("Lowest " << children[i].resizeLowestBound);
                ERROR("Highest " << children[i].resizeHighestBound);

                ERROR("Resizing " << children[i].resizeLowestBound << "|" << children[i].resizeHighestBound);
            } if(r == EDITOR_WINDOW_STATE_SUBDIVIDE) {
                if(childrenBufferSize <= childrenCount) {
                    childrenBufferSize += 10;
                    EditorView* temp = new EditorView[childrenBufferSize];
                    memcpy(temp, children, childrenCount * sizeof(EditorView));
                    delete [] children;
                    children = temp;
                }

                EditorView child;
                child.init();
                child.width = children[i].width;
                children[i].width = offset + ((children[i].width - offset) / 2.0f);
                ERROR("Subdividing " << child.width << "|" << children[i].width);
                if(i == childrenCount - 1) {

                } else {
                    memcpy(&children[i + 2], &children[i + 1], (childrenCount - i) * sizeof(EditorView));
                }

                children[childrenCount++] = child;
                ERROR("Children count " << childrenCount);
            }

            if(children[i].lowestBound < crossLowestBound)
                crossLowestBound = children[i].lowestBound;
            if(children[i].highestBound > crossHighestBound) 
                crossHighestBound = children[i].highestBound;

            offset = children[i].width;
        }
    } else {
        lowestBound = width;
        highestBound = x;

        crossLowestBound = height;
        crossHighestBound = y;
    }

    if(keyboard.leftControl == KEY_STATE_HOLD && cursor.leftButtonState == MOUSE_BUTTON_STATE_PRESS && result != EDITOR_WINDOW_STATE_SPLIT && width - x > 0.2f) {
        if(cursorY > x && cursorY < width && cursorX > y && cursorX < height) {
            if(keyboard.leftShift == KEY_STATE_HOLD && result != EDITOR_WINDOW_STATE_SUBDIVIDE) {
                result = EDITOR_WINDOW_STATE_SUBDIVIDE;
            } else {
                result = EDITOR_WINDOW_STATE_SPLIT;
                ERROR("Splitting " << x << "|" << y << ":" << width << "|" << height);

                if(childrenBufferSize < 2) {
                    init(10);
                }

                children[0].init();
                children[0].width = height - (height - y) / 2.0f;
                children[0].childrenCount = 0;
                children[1].init();
                children[1].width = (height);
                children[1].childrenCount = 0;
                childrenCount = 2;
                ERROR("Created " << children[0].width << " & " << children[1].width);
            }
        }
    }

    // Render the quad
    if(childrenCount == 0) {
        float w = width;
        float tileX = tileInfo.width;
        float tileY = tileInfo.height;
        // Flip the values if the flow is flipped
        if(flow == EDITOR_WINDOW_FLOW_X) {
            std::swap(x, y);
            std::swap(height, w);
            std::swap(tileX, tileY);
        } 
        //ERROR("Rendering on : " << x << "|" << y << " with: " << width << "|" << height);
        glm::vec4 transform = glm::vec4(x, y, w, height);
        ShaderLib::uniform_vec4(program, "transform", &transform.x);
        glm::vec4 color = glm::vec4(0.5f, x, y, 1.0f);
        ShaderLib::uniform_vec4(program, "color", &color.x);

        RenderLib::draw_triangle_strip(4);

        std::string label = "##" + std::to_string(id);
        ImGui::Begin(label.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

        ImGui::SetWindowPos(ImVec2(x * winWidth, (y) * winHeight + 19));
        ImGui::SetWindowSize(ImVec2((w - x) * winWidth, (height - y) * winHeight));

        if(tile != nullptr)
            tile->draw(cursor, tileInfo);

        ImGui::End();
    }

    return result;
}

float EditorView::find_bound_lowest(uint32_t flow) {
    float result = width;
    if(children != nullptr) {
        for(uint32_t i = 0; i < childrenCount; i++) {
            if(children[i].children != nullptr) {
                for(uint32_t x = 0; x < children[i].childrenCount; x++) {
                    float r = children[i].children[x].find_bound_lowest(flow);
                    if(r < result) {
                        result = r;
                    }
                }
            }
        }
    }

    return result;
}

float EditorView::find_bound_highest(uint32_t flow, float initial) {
    float result = initial;
    for(uint32_t i = 0; i < childrenCount; i++) {
        if(children[i].childrenCount > 1) {
            float r = children[i].children[children[i].childrenCount - 1].find_bound_highest(flow, children[i].children[children[i].childrenCount - 2].width);
            if(r > result) {
                result = r;
            }
        }
    }

    return result;
}

void EditorView::terminate() {
    delete [] tile;
}


void EditorView::assign(WindowTile* tile) {
    this->tile = tile;
    this->tile->init();
}

void EditorView::resize_callback(uint32_t width, uint32_t height) {
    if(tile != nullptr)
        tile->resize_callback(width, height);

    for(uint32_t i = 0; i < childrenCount; i++)
        children[i].resize_callback(width, height);
}