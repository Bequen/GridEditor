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
    childrenBufferSize = 1;
    children = (EditorView*)malloc(childrenBufferSize * sizeof(EditorView));

    tile = nullptr;
    childrenCount = 0;
}

void EditorView::init(uint32_t bufferSize) {
    id = EditorView::lastId++;
    childrenBufferSize = bufferSize;
    children = (EditorView*)malloc(childrenBufferSize * sizeof(EditorView));

    tile = nullptr;
    childrenCount = 0;
}

void EditorView::update(Cursor cursor, float offsetX, float offsetY, float height, uint32_t program, uint32_t flow, uint32_t winWidth, uint32_t winHeight) {
    float x = offsetX;
    float y = offsetY;

    // If the user pressed the mouse meaning he wants to interact
    // The exception is interacting with the first window, can't resize that one, obviously
    if(cursor.leftButtonState == STATE_PRESS && width < 1.0f)
        if(cursor.cursorY < width && cursor.cursorY > width - 0.05f)
            state = EDITOR_WINDOW_STATE_RESIZE;
    else if(cursor.leftButtonState == STATE_NONE)
        state = EDITOR_WINDOW_STATE_NONE;

    // Now let's run through the children and update those
    {
        float offset = 0.0f;
        // Update each one
        for(uint32_t i = 0; i < childrenCount; i++) {
            assert_msg(children, "Children are NOT initialized");
            // Move the window to the cursor position
            if(children[i].state == EDITOR_WINDOW_STATE_RESIZE) {
                if(cursor.cursorX > children[i + 1].width - 0.1f)
                    children[i].width = children[i + 1].width - 0.1f;
                else if(cursor.cursorX < x + 0.1f)
                    children[i].width = x + 0.1f;
                else
                    children[i].width = cursor.cursorX;

                if(children[i].tile != nullptr)
                    children[i].resize_callback(children[i].width * winWidth, width * winHeight);
                ERROR("Resizing on " << x << " to " << children[i].width);
            }

            // There is no way they can be in same direction as parent, so switching the x & y turns the direction
            children[i].update(cursor, offset, offsetX, width, program, 1 - flow, winWidth, winHeight);
            offset = offsetX + children[i].width;
        }
    }

    // Render the quad
    if(childrenCount == 0) {
        float w = width;
        // Flip the values if the flow is flipped
        if(flow == EDITOR_WINDOW_FLOW_X) {
            std::swap(x, y);
            std::swap(height, w);
        }
        //ERROR("Rendering on : " << x << "|" << y << " with: " << w << "|" << height);
        glm::vec4 transform = glm::vec4(x, y, w, height);
        ShaderLib::uniform_vec4(program, "transform", &transform.x);
        glm::vec4 color = glm::vec4(0.5f, x, y, 1.0f);
        ShaderLib::uniform_vec4(program, "color", &color.x);

        RenderLib::draw_triangle_strip(4);

        tileInfo.height = height - y;
        tileInfo.width = w - x;
        tileInfo.x = x;
        tileInfo.y = y;
        std::string label = std::to_string(id);
        ImGui::Begin(label.c_str(), NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);

        if(y == 1.0f) {
            ImGui::SetWindowPos(ImVec2(x * winWidth, (y) * winHeight + 19));
        } else {
            ImGui::SetWindowPos(ImVec2(x * winWidth, (y) * winHeight + 19));
        } ImGui::SetWindowSize(ImVec2((w - x) * winWidth, (height - y) * winHeight));

        if(tile != nullptr)
            tile->draw(cursor, tileInfo);

        ImGui::End();
    }
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