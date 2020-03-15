#pragma once

#include "Camera.h"
#include "System/Window.h"
#include "System/Voxels/Grid.h"
#include "Rendering/RenderingPipeline.h"
#include "Color.h"
#include "Ray.h"
#include "Rendering/Light.h"
#include "System/Scene.h"
#include "Editor/Tiles/Viewport.h"
#include "Keyboard.h"
#include "EditorView.h"

/**
 * @brief  Editor is the main class controlling the app behaviour
 * @note   Stores the Window Tile Manager and everything
 * @retval None
 */
class Editor {
    public:
        Window window;

        uint32_t drawQuad;
        uint32_t deferredProgram;

        EditorView editorWindow;

        uint32_t windowProgram;
        uint32_t windowQuad;

        Cursor cursor;
        Scene scene;

        double* deltaTime;
        Keyboard keyboard;

        void init();
        void update();
        void terminate();

        void update_cursor();
        void update_keyboard();
        void update_key(uint32_t key, uint32_t& state);

        void draw_menubar();
        void draw_ui();
        void draw_palette();
        void draw_toolbar();
        void draw_scene_setup();

        void resize_callback(int32_t width, int32_t height);
};