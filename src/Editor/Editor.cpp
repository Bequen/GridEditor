#include "Editor.h"

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Rendering/RenderLib.h"
#include <avg/Debug.h>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Rendering/TextureLib.h"
#include "Rendering/ShaderLib.h"
#include <csignal>

void Editor::init() {
    MESSAGE("Starting the editor initialization");
    viewport.window = window;
    viewport.init();

    deferredProgram = ShaderLib::program_create("deferred");
    drawQuad = RenderLib::create_render_quad();
}

void Editor::update() {
    viewport.update();
    RenderLib::bind_vertex_array(drawQuad);
    ShaderLib::program_use(deferredProgram);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, viewport.framebuffer.texture);
    ShaderLib::uniform_int32(deferredProgram, "color", 3);

    RenderLib::draw_triangle_strip(4);

    draw_ui();
}

void Editor::draw_ui() {
    draw_palette();
    draw_toolbar();
    draw_scene_setup();
}

void Editor::draw_palette() {
    ImGui::Begin("Pallette", nullptr);

    ImGui::BeginChild("PickerWindow", ImVec2(200.0f, 200.0f), true);
    // Stupid workaround
    if(viewport.colorSelected > 255)
        viewport.colorSelected = 1;
    if(ImGui::ColorPicker3("picker", &viewport.palette[viewport.colorSelected].r, ImGuiColorEditFlags_PickerHueWheel))
        viewport.update_palette();
    ImGui::EndChild();

    ImGui::BeginChild("PaletteWindow", ImVec2(0.0f, 0.0f), true);
    float buttonSize = 20.0f;
    float padding = 5.0f;

    float size = ImGui::GetWindowContentRegionWidth();
    int rowSize = (int)std::floor(size / (buttonSize + padding));
    for(uint32_t i = 0; i < 256; i++) {
        const char* label = std::to_string(i).c_str();
        if(rowSize > 0) {
            if(i % rowSize)
                ImGui::SameLine((i % rowSize) * (buttonSize + padding) + padding * 2);
            if(ImGui::ColorButton(label, ImVec4(viewport.palette[i].r, viewport.palette[i].g, viewport.palette[i].b, 1.0f), 0, ImVec2(buttonSize, buttonSize))) {
                viewport.colorSelected = i;
                viewport.colorCache = i;
            }
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void Editor::draw_toolbar() {
    ImGui::Begin("Toolbar");

    const char* items[] = { "Cube", "Line", "Circle" };

    if (ImGui::BeginCombo("##combo", items[viewport.rectangle]))
    {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
        {
            bool is_selected = (viewport.rectangle == n);
            if (ImGui::Selectable(items[n], is_selected))
                viewport.rectangle = n;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    ImGui::End();
}

void Editor::draw_scene_setup() {
    ImGui::Begin("Scene Setup");

    ImGui::BeginChild("Lighting", ImVec2(200.0f, 200.0f), true);
    if(ImGui::ColorPicker4("Sky Color", &viewport.scene.skyColor.r)) {
        viewport.render.skyColor = viewport.scene.skyColor;
        viewport.update_sky_color();
    }
    ImGui::EndChild();

    ImGui::BeginChild("Scene Lights");
    for(uint32_t i = 0; i < viewport.scene.lightCount; i++) {
        char* buffer = new char[256];
        sprintf(buffer, "LightLabel%i", i);

        ImGui::BeginChild(buffer, ImVec2(0.0f, 100.0f), true);
        ImGui::Text("Light %i", i);
        if(ImGui::InputFloat3("Position", &viewport.scene.lights[i].position.x, 3)) {
            viewport.update_lights();
            ERROR("Updating Lights");
        } if(ImGui::InputFloat3("Direction", &viewport.scene.lights[i].direction.x, 3)) {
            viewport.update_lights();
            ERROR("Updating Lights");
        }
        ImGui::EndChild();

        delete [] buffer;
    }

    {
        ImGui::BeginChild("AddLightLabel", ImVec2(200.0f, 0.0f));
        if(ImGui::Button("AddLightButton")) {
            viewport.scene.lightCount++;
        }
        ImGui::EndChild();
    }

    ImGui::EndChild();

    ImGui::End();
}

void Editor::terminate() {

}

void Editor::resize_callback(int32_t width, int32_t height) {
    this->window.width = width;
    this->window.height = height;
    viewport.window.width = width;
    viewport.window.height = height;

    glViewport(0, 0, width, height);
    viewport.camera->projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
    viewport.framebuffer.width = width;
    viewport.framebuffer.height = height;

    glDeleteTextures(1, &viewport.framebuffer.texture);
    glDeleteRenderbuffers(1, &viewport.framebuffer.depth);
    glBindFramebuffer(GL_FRAMEBUFFER, viewport.framebuffer.framebuffer);

    uint32_t colorAttachment = TextureLib::create_texture_2d(GL_TEXTURE_2D, width, height, GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA, nullptr);
    TextureLib::framebuffer_attachment(colorAttachment, GL_TEXTURE_2D, GL_COLOR_ATTACHMENT0);
    viewport.framebuffer.texture = colorAttachment;

    glGenRenderbuffers(1, &viewport.framebuffer.depth);
    glBindRenderbuffer(GL_RENDERBUFFER, viewport.framebuffer.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, viewport.framebuffer.depth);
}