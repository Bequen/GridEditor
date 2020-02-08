#include "Editor.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Rendering/RenderLib.h"
#include <avg/Debug.h>
#include <avg/Random/PermutationTable.h>
#include <avg/Random/PerlinNoise.h>
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
    scene.init(1);

    scene.voxelVAO = RenderLib::create_voxel();
    scene.boxShader = ShaderLib::program_create("box");

    deferredProgram = ShaderLib::program_create("deferred");
    drawQuad = RenderLib::create_render_quad();

    windowQuad = RenderLib::create_render_quad();
    windowProgram = ShaderLib::program_create("window");

    // Basic layout
    // TODO Make layout saveable
    editorWindow.init(10);
    editorWindow.width = 1.0f;
    editorWindow.tileInfo.width = 1.0f;
    editorWindow.tileInfo.height = 1.0f;
    editorWindow.tileInfo.x = 0.0f;
    editorWindow.tileInfo.y = 0.0f;

    editorWindow.children[0].init(10);
    editorWindow.children[0].width = 0.5f;

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

    editorWindow.children[0].assign(new Viewport(&scene, window, deltaTime));
}

void Editor::update() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderLib::update();
    RenderLib::bind_vertex_array(drawQuad);
    ShaderLib::program_use(deferredProgram);

    draw_menubar();

    update_cursor();
    update_keyboard();
    editorWindow.update(cursor, keyboard, 0.0f, 0.0f, 1.0f, windowProgram, EDITOR_WINDOW_FLOW_X, window.width, window.height - 19);
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

void Editor::draw_menubar() {
    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            ImGui::MenuItem("Load", NULL);
            ImGui::MenuItem("Save", NULL);
            ImGui::MenuItem("Quit", NULL);
            ImGui::EndMenu();
        }
    }

    ImGui::EndMainMenuBar();
}

void Editor::terminate() {

}

void Editor::resize_callback(int32_t width, int32_t height) {
    this->window.width = width;
    this->window.height = height;
    glViewport(0, 0, width, height);
    editorWindow.resize_callback(width, height);
}