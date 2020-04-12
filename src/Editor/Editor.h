#pragma once

#include "Camera.h"
#include "System/Window.h"
#include "System/Voxels/Grid.h"
#include "Rendering/RenderingPipeline.h"
#include "Color.h"
#include "Ray.h"
#include "Rendering/Light.h"
#include "Editor/Scene.h"
#include "Editor/Tiles/Viewport.h"
#include "EditorView.h"
#include "RenderInfo.h"
#include "WindowManager/WindowManager.h"

/**
 * @brief  Editor is the main class controlling the app behaviour
 * @note   Stores the Window Tile Manager and everything
 * @retval None
 */
class Editor {
    public:
        uint32_t drawQuad;
        uint32_t deferredProgram;

        EditorView editorWindow;
        WindowManager windowManager;

        Scene scene;
        // Rendering Pipeline is probably deprecated by now
        RenderingPipeline render;
        RenderInfo renderInfo;

        void init();
        void update();
        void terminate();

        void draw_menubar();
        void draw_statusbar();

        void draw_ui();
        void draw_palette();
        void draw_toolbar();
        void draw_scene_setup();

        void resize_callback(int32_t width, int32_t height);
};