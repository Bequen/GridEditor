#pragma once

#include <cstdint>

#include "Window.h"

class Application {
    Window window;

    public:
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