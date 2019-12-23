#include "Application.h"

#include <avg/Debug.h>

#include "Rendering/RenderLib.h"

void Application::init() {
    MESSAGE("Application initialization has started");

    window.init(PROJECT_NAME, 720, 480);
    editor.init();

    editor.deltaTime = &deltaTime;
    editor.window = window;

    MESSAGE("Application initialization has finished");
}

void Application::update() {
    MESSAGE("Update loop has started");

    double lastTime = 0;

    while(!window.should_close()) {
        deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        // Insert tick code
        RenderLib::update();

        editor.update();

        window.update();
    }
    MESSAGE("Update loop has ended");
}

void Application::terminate() {
    ERROR("Terminating the application");
    editor.terminate();

    window.terminate();
}