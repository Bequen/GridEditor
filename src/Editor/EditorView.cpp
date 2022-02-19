#include "EditorView.h"

#include <cstdlib>
#include <avg/Debug.h>
#include <cstring>
#include <cstdio>

#include "Rendering/RenderLib.h"
#include "Rendering/ShaderLib.h"
#include "Editor/Tiles/Viewport.h"
#include <imgui/imgui.h>

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

// How does it work?
// First, we run through all the nodes and check, if the cursor is not
// close enough to border for resizing. If so, change state to resize,
// which will then be handled by parent, that has all the children in
// one row. If some of these children have another children, simple
// rotate by 90deg to create immersion
uint32_t EditorView::update(float offsetX, float offsetY, float height, uint32_t flow) {
    // Resets the state
    uint32_t result = EDITOR_WINDOW_STATE_NONE;
    
    float x = offsetX;
    float y = offsetY;

    float cursorX = (Input.mouseLastX + 1.0f) / 2.0f;
    float cursorY = (-1.0 * Input.mouseLastY + 1.0f) / 2.0f;

    // If the flow is opposite, switch the values
    if(flow == EDITOR_WINDOW_FLOW_X) {
        std::swap(cursorX, cursorY);
    }

    // If the user pressed the mouse meaning he wants to interact
    // The exception is interacting with the last window, can't resize that one, obviously
    if(Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_PRESS && width < 1.0f) {
        if(cursorX < width && cursorX > width - 0.01f) {
            state = EDITOR_WINDOW_STATE_RESIZE;
            result = EDITOR_WINDOW_STATE_RESIZE;
        }
    } else if(Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_NONE && state == EDITOR_WINDOW_STATE_RESIZE) {
        state = EDITOR_WINDOW_STATE_NONE;
        resize_callback(Input.windowWidth, Input.windowHeight);
    }


    // Now let's run through the children and update those
    if(childrenCount > 0) {
        float offset = offsetY;

        crossLowestBound = height;
        crossHighestBound = y;

        highestBound = children[childrenCount - 1].crossHighestBound;
        lowestBound = children[0].crossLowestBound;

        assert_msg(children, "Children are NOT initialized");
        for(uint32_t i = 0; i < childrenCount; i++) {
            // Move the window to the cursor position
            
            if(children[i].state == EDITOR_WINDOW_STATE_RESIZE) {
                //ERROR(children[i].resizeHighestBound << ":" << children[i].resizeLowestBound << "=" << cursorX)
                ERROR("Cursor: " << cursorY);
                if(cursorY > children[i].resizeHighestBound - 0.05f)
                    children[i].width = children[i].resizeHighestBound - 0.05f;
                else if(cursorY < children[i].resizeLowestBound + 0.05f)
                    children[i].width = children[i].resizeLowestBound + 0.05f;
                else
                    children[i].width = cursorY;
            }

            // There is no way they can be in same direction as parent, so switching the x & y turns the direction
            uint32_t r = children[i].update(offset, x, width, 1 - flow);
            // Workaround, because if upcoming child won't be in split state, it would overrite
            /*if(r == EDITOR_WINDOW_STATE_SPLIT) {
                MESSAGE("Splitting");
                result = EDITOR_WINDOW_STATE_SPLIT;
            }  */if(r == EDITOR_WINDOW_STATE_RESIZE) {
                // We need to find bounds for resizing
                // It is simple tree traversal algorithm that looks for borders that are closest to resized window
                children[i].resizeLowestBound = children[i].find_bound_highest(flow, offset);
                children[i].resizeHighestBound = children[i + 1].find_bound_lowest(flow);
            }/*  if(r == EDITOR_WINDOW_STATE_SUBDIVIDE) {
                MESSAGE("Subdividing");
                if(childrenBufferSize <= childrenCount)
                    resize_buffer(childrenBufferSize + 10);

                EditorView child;
                child.init();
                child.width = children[i].width;
                // Change the width of the child so the new split can fit
                children[i].width = offset + ((children[i].width - offset) / 2.0f);

                // In case the child that we are splitting is not the last in the row, we need to insert, instead of just push
                if(i != childrenCount - 1)
                    memcpy(&children[i + 2], &children[i + 1], (childrenCount - i) * sizeof(EditorView));

                children[childrenCount++] = child;
            }
 */
            /* if(children[i].lowestBound < crossLowestBound)
                crossLowestBound = children[i].lowestBound;
            if(children[i].highestBound > crossHighestBound) 
                crossHighestBound = children[i].highestBound; */

            // Changes the offset for the next window
            offset = children[i].width;
        }
    } /* else {
        lowestBound = width;
        highestBound = x;

        crossLowestBound = height;
        crossHighestBound = y;
    } */

    /* if( Input.get(GLFW_KEY_LEFT_CONTROL) == KEY_STATE_HELD && 
        Input.get(GLFW_MOUSE_BUTTON_1) == KEY_STATE_PRESS && 
        result != EDITOR_WINDOW_STATE_SPLIT && width - x > 0.1f) {

        if(cursorY > x && cursorY < x + 0.05f && cursorX > y && cursorX < height) {
            if(Input.get(GLFW_KEY_LEFT_SHIFT) == KEY_STATE_HELD && result != EDITOR_WINDOW_STATE_SUBDIVIDE) {
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
    } */

    // Render the quad
    if(childrenCount == 0)
        draw(x, y, height, flow);

    return result;
}

void EditorView::draw(float x, float y, float height, uint32_t flow) {
    float w = width;
    float tileX = tileInfo.width;
    float tileY = tileInfo.height;
    // Flip the values if the flow is flipped
    if(flow == EDITOR_WINDOW_FLOW_X) {
        std::swap(x, y);
        std::swap(height, w);
        std::swap(tileX, tileY);
        //std::swap(tileInfo.width, tileInfo.height);
    } 

    std::string label = "##" + std::to_string(id);
    ImGui::Begin(label.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

    ImGui::SetWindowPos(ImVec2(x * Input.windowWidth, (y) * Input.windowHeight));
    ImGui::SetWindowSize(ImVec2((w - x) * Input.windowWidth, (height - y) * Input.windowHeight));


    /* if(tile != nullptr)
        tile->draw(tileInfo); */

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


void EditorView::assign(WindowTile* tile) {
    this->tile = tile;
    if(tile != nullptr)
        this->tile->init();
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
    
void EditorView::insert_window(EditorView window, uint32_t index) {
    assert_msg(this != nullptr, "You were trying to insert window into nullptr");
    MESSAGE("Inserting window at " << index);

    // If there is already a child, it is safe to add another
    if(childrenCount > 0 && children != nullptr) {
        MESSAGE("Adding children");
        // Fill up the tileInfo;
        WindowTileInfo info;

        float windowWidth = 0.0f;
        if(index > 0) {
            windowWidth = children[index].width + (children[index].width - children[index - 1].width) / 2.0f;
            info.x = children[index - 1].width;
            info.width = windowWidth - info.x;
        } else {
            windowWidth = children[index].width / 2.0f;
            info.x = tileInfo.y;
            WARNING(tileInfo.x);
            info.width = windowWidth - info.x;
        }
        info.y = tileInfo.x;
        info.height = width;

        window.tileInfo = info;
        window.tile = nullptr;
        window.width = windowWidth;
        MESSAGE("Width previous: " << children[index].width << ":" << childrenCount << "|" << index);

        memmove(&children[index + 1], &children[index], (childrenCount - index) * sizeof(EditorView));
        MESSAGE("Width " << info.x);
        children[index] = window;
        childrenCount++;
    }

    // Otherwise, we need to prepare it 
    else {
        MESSAGE("Resizing");
        // If the buffer is too small to hold the children
        if(childrenBufferSize < 2) {
            if(children)
                delete [] children;
            init(4);
        }

        // Get the parent instance to copy as the first child
        EditorView parent = *this;
        parent.children = nullptr;
        parent.childrenCount = 0;
        parent.childrenBufferSize = 0;

        // Make the index bounding
        if(index > 1)
            index = 1;

        children[index] = window;
        children[1 - index] = parent;

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
        ERROR(tileInfo.y << " + " << tileInfo.height / 2.0f << " = " << children[0].width << " : " << children[1].width);

        childrenCount = 2;
    }
}

void EditorView::drop_window(EditorView view, uint32_t flow) {
    MESSAGE("Dropping window");
    double x = Input.mouseLastX;
    double y = Input.mouseLastY;

    if(flow == EDITOR_WINDOW_FLOW_Y)
        std::swap(x, y);
    for(uint32_t i = 0; i < childrenCount; i++) {
        if(children[i].childrenCount == 0) {
            // If the cursor is hovering over the window
            if(children[i].width < x) {
                insert_window(view, i);
                break;
            }
        } else {
            if(children[i].width < x) {
                children[i].drop_window(view, 1 - flow);
                break;
            }
        }
    }
}
