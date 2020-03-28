#include "EditorView.h"

#include <cstdlib>
#include <avg/Debug.h>
#include <cstring>
#include <cstdio>

#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"
#include "Editor/Tiles/Viewport.h"
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

    float cursorX = cursor.cursorX / winWidth;
    float cursorY = (cursor.cursorY - MENUBAR_HEIGHT) / winHeight;
    
    tileInfo.x = x;
    tileInfo.y = y;
    tileInfo.width = width - x;
    tileInfo.height = height - y;

    if(flow == EDITOR_WINDOW_FLOW_Y) {
        std::swap(cursorX, cursorY);
        std::swap(tileInfo.width, tileInfo.height);
    }

    // If the user pressed the mouse meaning he wants to interact
    // The exception is interacting with the first window, can't resize that one, obviously
    if(cursor.leftButtonState == STATE_PRESS && width < 1.0f) {
        if(cursorY < width && cursorY > width - 0.01f) {
            state = EDITOR_WINDOW_STATE_RESIZE;
            result = EDITOR_WINDOW_STATE_RESIZE;
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
            uint32_t r = children[i].update(cursor, keyboard, offset, x, width, program, 1 - flow, winWidth, winHeight);
            // Workaround, because if upcoming child won't be in split state, it would overrite
            if(r == EDITOR_WINDOW_STATE_SPLIT)
                result = EDITOR_WINDOW_STATE_SPLIT;
            if(r == EDITOR_WINDOW_STATE_RESIZE) {
                children[i].resizeLowestBound = children[i].find_bound_highest(flow, offset);
                children[i].resizeHighestBound = children[i + 1].find_bound_lowest(flow);
            } if(r == EDITOR_WINDOW_STATE_SUBDIVIDE) {
                if(childrenBufferSize <= childrenCount)
                    resize_buffer(childrenBufferSize + 10);

                EditorView child;
                child.init();
                child.width = children[i].width;
                children[i].width = offset + ((children[i].width - offset) / 2.0f);

                if(i != childrenCount - 1)
                    memcpy(&children[i + 2], &children[i + 1], (childrenCount - i) * sizeof(EditorView));

                children[childrenCount++] = child;
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
        if(cursorY > x && cursorY < x + 0.05f && cursorX > y && cursorX < height) {
            if(keyboard.leftShift == KEY_STATE_HOLD && result != EDITOR_WINDOW_STATE_SUBDIVIDE) {
                result = EDITOR_WINDOW_STATE_SUBDIVIDE;
            } else {
                result = EDITOR_WINDOW_STATE_SPLIT;

                if(childrenBufferSize < 2)
                    resize_buffer(childrenBufferSize + 10);

                children[0].init();
                children[0].width = height - (height - y) / 2.0f;
                children[1].init();
                children[1].width = (height);
                childrenCount = 2;
            }
        }
    }

    // Render the quad
    if(childrenCount == 0)
        render(cursor, x, y, height, flow, winWidth, winHeight);

    return result;
}

void EditorView::render(Cursor cursor, float x, float y, float height, uint32_t flow, uint32_t winWidth, uint32_t winHeight) {
    float w = width;
    float tileX = tileInfo.width;
    float tileY = tileInfo.height;
    // Flip the values if the flow is flipped
    if(flow == EDITOR_WINDOW_FLOW_X) {
        std::swap(x, y);
        std::swap(height, w);
        std::swap(tileX, tileY);
        std::swap(tileInfo.width, tileInfo.height);
    } 

    std::string label = "##" + std::to_string(id);
    ImGui::Begin(label.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    ImGui::SetWindowPos(ImVec2(x * winWidth, (y) * winHeight + 19));
    ImGui::SetWindowSize(ImVec2((w - x) * winWidth, (height - y) * winHeight));


    if(tile != nullptr)
        tile->draw(cursor, tileInfo);

    ImGui::End();
}

void EditorView::resize_buffer(uint32_t newBufferSize) {
    childrenBufferSize = newBufferSize;
    EditorView* temp = new EditorView[childrenBufferSize];
    memcpy(temp, children, childrenCount * sizeof(EditorView));

    delete [] children;
    children = temp;
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


void EditorView::assign(WindowTile* tile, Window* window) {
    this->tile = tile;
    this->tile->init();
    this->tile->window = window;
}

void EditorView::resize_callback(uint32_t width, uint32_t height) {
    if(tile != nullptr)
        tile->resize_callback(width, height);

    for(uint32_t i = 0; i < childrenCount; i++)
        children[i].resize_callback(width, height);
}

void EditorView::refresh() {
    if(tile != nullptr)
        tile->refresh();
    for(uint32_t i = 0; i < childrenCount; i++)
        children[i].refresh();
}