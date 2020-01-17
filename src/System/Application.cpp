#include "Application.h"

#include <avg/Debug.h>

#include "Rendering/RenderLib.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

void Application::init() {
    MESSAGE("Application initialization has started");

    window.init(PROJECT_NAME, 720, 480);

    editor.deltaTime = &deltaTime;
    editor.window = window;
    editor.init();

    init_imgui();

    MESSAGE("Application initialization has finished");
}

void Application::init_imgui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 440");
}

void Application::update() {
    MESSAGE("Update loop has started");

    double lastTime = 0;

    while(!window.should_close()) {
        deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        int32_t width, height;
        glfwGetFramebufferSize(window.window, &width, &height);
        editor.resize_callback(width, height);
        if(width != editor.window.width || height != editor.window.height) {

            window.width = width;
            window.height = height;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Insert tick code
        RenderLib::update();

        editor.update();

        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window.window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        //glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.update();
    }
    MESSAGE("Update loop has ended");
}

void Application::terminate() {
    ERROR("Terminating the application");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    editor.terminate();

    window.terminate();
}