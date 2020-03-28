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
#include "Editor/Tiles/PaletteTile.h"
#include "Editor/Tiles/PerformanceMonitor.h"
#include "Editor/Tiles/SceneSetupTile.h"
#include "System/ContentPipeline.h"
#include "Editor/Tiles/TerminalTile.h"
#include <csignal>

void Editor::init() {
    MESSAGE("Starting the editor initialization");
    scene = Scene();
    scene.init(10);

    scene.colorSelected = 2;
    scene.add_grid(_Grid(32), "Grid1");

    renderInfo.voxelVAO = RenderLib::create_voxel();
    renderInfo.boxProgram = ShaderLib::program_create("box");

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
    editorWindow.children[0].childrenCount = 2;
    editorWindow.children[0].width = 0.5f;

    editorWindow.children[0].children[0].init();
    editorWindow.children[0].children[0].width = 0.75f;
    editorWindow.children[0].children[0].assign(new Viewport(&scene, window, deltaTime, renderInfo), &window);

    editorWindow.children[0].children[1].init();
    editorWindow.children[0].children[1].width = 1.0f;
    editorWindow.children[0].children[1].assign(new TerminalTile(&scene), &window);

    editorWindow.children[1].init(10);
    editorWindow.children[1].childrenCount = 2;
    editorWindow.children[1].width = 1.0f;
    editorWindow.children[1].children[0].init();
    editorWindow.children[1].children[0].width = 0.5f;
    editorWindow.children[1].children[0].assign(new PaletteTile(&scene), &window);
    editorWindow.children[1].children[1].init();
    editorWindow.children[1].children[1].width = 1.0f;
    editorWindow.children[1].children[1].assign(new SceneSetupTile(&scene), &window);

    editorWindow.childrenCount = 2;
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
    update_key(GLFW_KEY_LEFT_ALT, keyboard.leftAlt);
    update_key(GLFW_KEY_LEFT_CONTROL, keyboard.leftControl);
    update_key(GLFW_KEY_LEFT_SHIFT, keyboard.leftShift);
}

void Editor::update_key(uint32_t key, uint32_t& state) {
    if(window.is_key_down(key)) {
        if(state == KEY_STATE_PRESS)
            state = KEY_STATE_HOLD;
        else if(state == KEY_STATE_NONE)
            state = KEY_STATE_PRESS;
    } else {
        state = KEY_STATE_NONE;
    } 
}

void Editor::draw_menubar() {
    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Load", NULL)) {
                scene = ContentPipeline::load_grid("/home/martin/test.grid");
                editorWindow.refresh();
            }
            if(ImGui::BeginMenu("Save")) {
                if(ImGui::MenuItem(".grid", NULL)) {
                    ContentPipeline::save_grid(scene, "/home/martin/test.grid");
                }
                ImGui::MenuItem(".obj", NULL);
                ImGui::EndMenu();
            }
            
            if(ImGui::MenuItem("Quit", NULL)) {
                glfwSetWindowShouldClose(window.window, 1);
            }
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