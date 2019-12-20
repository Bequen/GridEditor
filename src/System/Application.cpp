#include "Application.h"

#include <avg/Debug.h>

#include "Rendering/RenderLib.h"

void Application::init() {
    MESSAGE("Application initialization has started");

    window.init(PROJECT_NAME, 720, 480);

    MESSAGE("Application initialization has finished");
}

void Application::update() {
    MESSAGE("Update loop has started");
    while(!window.should_close()) {
        // Insert tick code
        RenderLib::update();

        window.update();
    }
    MESSAGE("Update loop has ended");
}

void Application::terminate() {

}