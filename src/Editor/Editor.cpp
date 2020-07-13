#include "Editor.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"

#include <avg/Debug.h>
#include <avg/Random/PermutationTable.h>
#include <avg/Random/PerlinNoise.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <csignal>

#include "Rendering/RenderLib.h"
#include "Rendering/TextureLib.h"
#include "Rendering/ShaderLib.h"
#include "Editor/Tiles/PaletteTile.h"
#include "System/ContentPipeline.h"
#include "Tiles/PaletteTile.h"
#include "Tiles/TimelineTile.h"
#include "Tiles/SceneExplorerTile.h"
#include "Tiles/PropertiesTile.h"

void Editor::init() {
    MESSAGE("Starting the editor initialization");
    scene = Scene();
    scene.init(10);

    RenderLib::init();

    #pragma region INPUT INITIALIZATION
    Input.init(32);

    Input.add_key({GLFW_KEY_TAB, 0});
    Input.add_key({GLFW_KEY_A, 0});
    Input.add_key({GLFW_KEY_R, 0});
    Input.add_key({GLFW_KEY_G, 0});
    Input.add_key({GLFW_KEY_P, 0});
    Input.add_key({GLFW_KEY_S, 0});
    Input.add_key({GLFW_KEY_Z, 0});
    Input.add_key({GLFW_KEY_B, 0});
    Input.add_key({GLFW_KEY_LEFT_SHIFT, 0});
    Input.add_key({GLFW_KEY_LEFT_CONTROL, 0});
    Input.add_key({GLFW_KEY_LEFT_ALT, 0});
    Input.add_key({GLFW_KEY_1, 0});
    Input.add_key({GLFW_KEY_3, 0});
    Input.add_key({GLFW_KEY_5, 0});
    Input.add_key({GLFW_KEY_7, 0});
    Input.add_key({GLFW_KEY_9, 0});
    Input.add_key({GLFW_KEY_DELETE, 0});

    Input.add_mouse_key({GLFW_MOUSE_BUTTON_1, 0});
    Input.add_mouse_key({GLFW_MOUSE_BUTTON_2, 0});
    Input.add_mouse_key({GLFW_MOUSE_BUTTON_3, 0});
    #pragma endregion

    scene.colorSelected = 2;
    scene.sceneGraph.add_child(SceneObject(OBJECT_TYPE_GRID, scene.add_grid(SceneGrid(32, 32, 32))));
    scene.selected = &scene.sceneGraph.children[0];

    #pragma region RENDERING PIPELINE INITIALIZATION
    renderInfo.voxelVAO = RenderLib::create_voxel();
    renderInfo.boxProgram = ShaderLib::program_create("boxes/gridBox");

    deferredProgram = ShaderLib::program_create("deferred");
    drawQuad = RenderLib::create_render_quad();

    renderInfo.quadProgram = ShaderLib::program_create("deferred");
    renderInfo.quadVAO = RenderLib::create_render_quad();
    renderInfo.skyProgram = ShaderLib::program_create("skybox");
    renderInfo.voxelProgram = ShaderLib::program_create("voxel");
    renderInfo.voxelSelectedProgram = ShaderLib::program_create("VoxelSelected");
    renderInfo.spriteProgram = ShaderLib::program_create("sprite");
    #pragma endregion

    #pragma region WINDOW MANAGER INITIALIZATION
    windowManager.init();
    windowManager.tiles.editors = new PaletteTile(&scene, renderInfo);
    windowManager.tiles.editors->init();
    windowManager.tiles.editorCount = 1;

    _WindowTile viewport;
    viewport.init();
    viewport.editors = new Viewport(&scene, renderInfo);
    viewport.editors->init();
    viewport.editorCount = 1;
    windowManager.tiles.insert_window(viewport, 1);
    
    _WindowTile timeline;
    timeline.init();
    timeline.editors = new TimelineTile(&scene, renderInfo);
    timeline.editors->init();
    timeline.editorCount = 1;
    windowManager.tiles.children[1].insert_window(timeline, 1);

    _WindowTile sceneExplorer;
    sceneExplorer.init();
    sceneExplorer.editors = new SceneExplorerTile(&scene, renderInfo);
    sceneExplorer.editors->init();
    sceneExplorer.editorCount = 1;
    windowManager.tiles.children[0].insert_window(sceneExplorer, 1);

    _WindowTile propertiesTile;
    propertiesTile.init();
    propertiesTile.editors = new PropertiesTile(&scene, renderInfo);
    propertiesTile.editors->init();
    propertiesTile.editorCount = 1;
    windowManager.tiles.insert_window(propertiesTile, 1);
    
    #pragma endregion
}

void Editor::update() {
    Input.update();
    render.update();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderLib::update();
    RenderLib::bind_vertex_array(drawQuad);
    ShaderLib::program_use(deferredProgram);

    draw_menubar();

    windowManager.update();
    windowManager.draw();
}


void Editor::draw_menubar() {
    if(ImGui::BeginMainMenuBar()) {
        if(ImGui::BeginMenu("File")) {
            if(ImGui::MenuItem("Load", NULL)) {
                scene = ContentPipeline::load_grid("/home/martin/test.grid");
                windowManager.refresh();
            }
            if(ImGui::BeginMenu("Save")) {
                if(ImGui::MenuItem(".grid", NULL)) {
                    ContentPipeline::save_grid(scene, "/home/martin/test.grid");
                }
                ImGui::MenuItem(".obj", NULL);
                ImGui::EndMenu();
            }
            
            if(ImGui::MenuItem("Quit", NULL)) {
                //glfwSetWindowShouldClose(window.window, 1);
            }
            ImGui::EndMenu();
        }
    }

    ImGui::EndMainMenuBar();
}

void Editor::terminate() {
    ShaderLib::program_delete(renderInfo.boxProgram);
    ShaderLib::program_delete(renderInfo.voxelProgram);
    ShaderLib::program_delete(renderInfo.skyProgram);
    ShaderLib::program_delete(renderInfo.quadProgram);

    ShaderLib::program_delete(deferredProgram);

    RenderLib::delete_vertex_array(renderInfo.quadVAO);
    RenderLib::delete_vertex_array(renderInfo.voxelVAO);
}

void Editor::resize_callback(int32_t width, int32_t height) {
    glViewport(0, 0, width, height);
    windowManager.resize_callback();
}