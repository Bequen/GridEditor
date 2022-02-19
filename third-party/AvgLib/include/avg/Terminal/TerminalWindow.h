#pragma once

#include <cstdint>

class TerminalWindow {
    public:
        char* name;

        char* buffer;

        int32_t width, height;
        int32_t padding;

        TerminalWindow* children;
        uint32_t childCount;

        uint32_t flow;

        TerminalWindow();
        TerminalWindow(char* name, int32_t width, int32_t height, int32_t padding);

        void set_size(uint32_t width, uint32_t height);
};
