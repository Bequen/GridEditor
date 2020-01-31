#pragma once

#include "Camera.h"
#include "System/Window.h"
#include "System/Grid.h"
#include "Rendering/RenderingPipeline.h"
#include "Color.h"
#include "Ray.h"
#include "Rendering/LightBuffer.h"
#include "Rendering/Light.h"
#include "System/Scene.h"
#include "Viewport.h"
#include "EditorView.h"

class Editor {
    public:
        Window window;
        //Viewport viewport;

        uint32_t drawQuad;
        uint32_t deferredProgram;

        EditorView editorWindow;

        uint32_t windowProgram;
        uint32_t windowQuad;

        Cursor cursor;
        Scene scene;

        double* deltaTime;

        void init();
        void update();
        void terminate();

        void update_cursor();

        void draw_menubar();
        void draw_ui();
        void draw_palette();
        void draw_toolbar();
        void draw_scene_setup();

        void resize_callback(int32_t width, int32_t height);
};