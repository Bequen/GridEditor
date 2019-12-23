#pragma once

#include <cstdint>

#include "Window.h"
#include "Rendering/RenderingPipeline.h"
#include "Editor/Editor.h"

/**
 * desc :   Where all the things happens
 * info :   Application class takes responsibility for entire runtime, it manages the main loop, initialization and even cleanup
 */ 
class Application {
    Window window;
    Editor editor;


    public:
        double deltaTime;

        /**
         * desc :   Initializes the application and every component it needs
         * info :   This function starts the whole application
         */
        void init();

        /**
         * desc :   Main update loop is stored here
         * info :   Main update loop is here and every other OnTick is called from here
         */ 
        void update();

        /**
         * desc :   Application termination and cleanup
         * info :   Where all the cleanup code goes
         */ 
        void terminate();
};