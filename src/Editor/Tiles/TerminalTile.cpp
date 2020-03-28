#include "TerminalTile.h"

#include <Rendering/ShaderLib.h>
#include <glad/glad.h>
#include <ImGUI/imgui.h>
#include <avg/StringLib.h>

void TerminalTile::init() {
    buffer = new char[1024];
    bufferSize = 1024;
    memset(buffer, 0, 1024);
    buffer[0] = '\0';
}

void TerminalTile::update() {

}

void TerminalTile::draw(Cursor cursor, WindowTileInfo tileInfo) {
    ImGui::BeginChild("TerminalWindow", ImVec2(0.0f, 0.0f), false);
    if(ImGui::InputText("Terminal", buffer, bufferSize, ImGuiInputTextFlags_EnterReturnsTrue)) {
        MESSAGE("Terminal command executed: " << buffer);
        solve_command();
        memset(buffer, 0, bufferSize);
    }

    ImGui::EndChild();
}

void TerminalTile::solve_command() {
    uint32_t splitCount;
    char** splits = avg::StringLib::split(buffer, ' ', &splitCount);

    if(splitCount > 0 && avg::StringLib::equals(splits[0], "object")) {
        if(splitCount > 1 && avg::StringLib::equals(splits[1], "new")) {
            if(splitCount > 2 && avg::StringLib::equals(splits[2], "grid")) {
                if(splitCount == 3) {
                    ERROR("Incomplete command");
                    return;
                }

                uint32_t valueCount = 0;
                char** values = avg::StringLib::split(splits[3], ',', &valueCount);

                if(valueCount == 1) {
                    remove_border(values[0]);
                    int32_t size = std::atoi(values[0] + 1);

                    scene->add_grid(_Grid(size), "new_grid");
                    MESSAGE("Added new grid of size " << size);
                }
            }
        }
    }
}

void TerminalTile::remove_border(char* str) {
    uint32_t i;
    for(i = 0; str[i] != '\0'; i++) {
        
    } str[i - 1] = '\0';
}

void TerminalTile::terminate() {

}

void TerminalTile::resize_callback(uint32_t width, uint32_t height) {

}

void TerminalTile::refresh() {
    
}