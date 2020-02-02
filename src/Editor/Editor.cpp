#include "Editor.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Rendering/RenderLib.h"
#include <avg/Debug.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Rendering/TextureLib.h"
#include "Rendering/ShaderLib.h"
#include "PaletteTile.h"
#include "SceneSetupTile.h"
#include <csignal>

void Editor::init() {
    MESSAGE("Starting the editor initialization");
    scene = Scene();
    scene.init();
    scene.grids[0].cache[0].set(glm::vec3(0.0f, 0.0f, 0.0f), 2);
    scene.colorSelected = 2;
    /* viewport.window = window;
    viewport.scene = &scene;
    viewport.init(); */

    deferredProgram = ShaderLib::program_create("deferred");
    drawQuad = RenderLib::create_render_quad();

    windowQuad = RenderLib::create_render_quad();
    windowProgram = ShaderLib::program_create("window");

    editorWindow.init(10);
    editorWindow.width = 1.0f;
    editorWindow.tileInfo.width = 1.0f;
    editorWindow.tileInfo.height = 1.0f;
    editorWindow.tileInfo.x = 0.0f;
    editorWindow.tileInfo.y = 0.0f;

    editorWindow.children[0].init(10);
    editorWindow.children[0].width = 0.5f;
    editorWindow.children[0].assign(new Viewport(&scene, window, deltaTime));

    

    editorWindow.children[1].init(10);
    editorWindow.children[1].childrenCount = 2;
    editorWindow.children[1].width = 1.0f;
    editorWindow.children[1].children[0].init();
    editorWindow.children[1].children[0].width = 0.5f;
    editorWindow.children[1].children[0].assign(new PaletteTile(&scene));
    editorWindow.children[1].children[1].init();
    editorWindow.children[1].children[1].width = 1.0f;
    editorWindow.children[1].children[1].assign(new SceneSetupTile(&scene));

    editorWindow.childrenCount = 2;

    scene.colorSelected = 2;
}

void Editor::update() {
    //viewport.update();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    RenderLib::update();
    RenderLib::bind_vertex_array(drawQuad);
    ShaderLib::program_use(deferredProgram);

/*     glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, viewport.framebuffer.texture);
    ShaderLib::uniform_int32(deferredProgram, "color", 3);

    RenderLib::draw_triangle_strip(4); */

    draw_ui();

    glDisable(GL_DEPTH_TEST);
    RenderLib::bind_vertex_array(windowQuad);
    ShaderLib::program_use(windowProgram);

    update_cursor();
    update_keyboard();
    //ERROR("Update");
    editorWindow.update_test(cursor, keyboard, 0.0f, 0.0f, 1.0f, windowProgram, EDITOR_WINDOW_FLOW_X, window.width, window.height - 19);
}

void Editor::update_cursor() {
    if(window.is_mouse_button_down(GLFW_MOUSE_BUTTON_1)) {
        if(cursor.leftButtonState == MOUSE_BUTTON_STATE_PRESS || cursor.leftButtonState == MOUSE_BUTTON_STATE_HOLD)
            cursor.leftButtonState = MOUSE_BUTTON_STATE_HOLD;
        else
            cursor.leftButtonState = MOUSE_BUTTON_STATE_PRESS;
    } else {
        cursor.leftButtonState = MOUSE_BUTTON_STATE_NONE;
    }

    double x, y;
    window.cursor_pos(&x, &y);

    cursor.deltaX = cursor.cursorX - x; 
    cursor.deltaY = cursor.cursorY - y;

    cursor.cursorX = x; 
    cursor.cursorY = y;
}

void Editor::update_keyboard() {
    // Left control
    if(window.is_key_down(GLFW_KEY_LEFT_CONTROL)) {
        if(keyboard.leftControl == KEY_STATE_PRESS)
            keyboard.leftControl = KEY_STATE_HOLD;
        else if(keyboard.leftControl == KEY_STATE_NONE)
            keyboard.leftControl = KEY_STATE_PRESS;
    } else {
        keyboard.leftControl = KEY_STATE_NONE;
    }



    // Left shift
    if(window.is_key_down(GLFW_KEY_LEFT_SHIFT)) {
        if(keyboard.leftShift == KEY_STATE_PRESS)
            keyboard.leftShift = KEY_STATE_HOLD;
        else if(keyboard.leftShift == KEY_STATE_NONE)
            keyboard.leftShift = KEY_STATE_PRESS;
    } else {
        keyboard.leftShift = KEY_STATE_NONE;
    } 



    // Left Alt
    if(window.is_key_down(GLFW_KEY_LEFT_ALT)) {
        if(keyboard.leftAlt == KEY_STATE_PRESS)
            keyboard.leftAlt = KEY_STATE_HOLD;
        else if(keyboard.leftAlt == KEY_STATE_NONE)
            keyboard.leftAlt = KEY_STATE_PRESS;
    } else {
        keyboard.leftAlt = KEY_STATE_NONE;
    } 
}

void Editor::draw_ui() {
    draw_menubar();
    draw_palette();
    draw_toolbar();
    draw_scene_setup();
}

void Editor::draw_menubar() {
    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Load", NULL);
            ImGui::MenuItem("Save", NULL);
            ImGui::MenuItem("Quit", NULL);
            ImGui::EndMenu();
        }
    }

    //ERROR("Size: " << ImGui::GetWindowHeight());

    ImGui::EndMainMenuBar();
}

void Editor::draw_palette() {
    /* ImGui::Begin("Pallette", nullptr);

    ImGui::BeginChild("PickerWindow", ImVec2(200.0f, 200.0f), true);
    // Stupid workaround
    if(viewport.colorSelected > 255)
        viewport.colorSelected = 1;
    if(ImGui::ColorPicker3("picker", &viewport.palette[viewport.colorSelected].r, ImGuiColorEditFlags_PickerHueWheel))
        viewport.update_palette();
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
            if(ImGui::ColorButton(label, ImVec4(viewport.palette[i].r, viewport.palette[i].g, viewport.palette[i].b, 1.0f), 0, ImVec2(buttonSize, buttonSize))) {
                viewport.colorSelected = i;
                viewport.colorCache = i;
            }
        }
    }
    ImGui::EndChild();

    ImGui::End(); */
}

void Editor::draw_toolbar() {
    /* ImGui::Begin("Toolbar");

    const char* items[] = { "Cube", "Line", "Circle" };

    if (ImGui::BeginCombo("##combo", items[viewport.rectangle]))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            bool is_selected = (viewport.rectangle == n);
            if (ImGui::Selectable(items[n], is_selected))
                viewport.rectangle = n;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::End(); */
}

void Editor::draw_scene_setup() {
    /* ImGui::Begin("Scene Setup");

    ImGui::BeginChild("Lighting", ImVec2(200.0f, 200.0f), true);
    if(ImGui::ColorPicker4("Sky Color", &viewport.scene.skyColor.r)) {
        viewport.render.skyColor = viewport.scene.skyColor;
        viewport.update_sky_color();
    }
    ImGui::EndChild();

    ImGui::BeginChild("Scene Lights");
    for(uint32_t i = 0; i < viewport.scene.lightCount; i++) {
        char* buffer = new char[256];
        sprintf(buffer, "LightLabel%i", i);

        ImGui::BeginChild(buffer, ImVec2(0.0f, 100.0f), true);
        ImGui::Text("Light %i", i);
        if(ImGui::InputFloat3("Position", &viewport.scene.lights[i].position.x, 3)) {
            viewport.update_lights();
            ERROR("Updating Lights");
        } if(ImGui::InputFloat3("Direction", &viewport.scene.lights[i].direction.x, 3)) {
            viewport.update_lights();
            ERROR("Updating Lights");
        }
        ImGui::EndChild();

        delete [] buffer;
    }

    {
        ImGui::BeginChild("AddLightLabel", ImVec2(200.0f, 0.0f));
        if(ImGui::Button("AddLightButton")) {
            viewport.scene.lightCount++;
        }
        ImGui::EndChild();
    }

    ImGui::EndChild();

    ImGui::End(); */
}

void Editor::terminate() {

}

void Editor::resize_callback(int32_t width, int32_t height) {
    this->window.width = width;
    this->window.height = height;
    glViewport(0, 0, width, height);
    editorWindow.resize_callback(width, height);
}