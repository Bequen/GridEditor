#pragma once

#include <cstdint>
#include <locale>
#include <iostream>
#include <termios.h>

#include "avg/Debug.h"
#include "TerminalWindow.h"

#define FLOW_HORIZONTAL 0x0000
#define FLOW_VERTICAL 0x0001

#define NO_MESSAGE 0x0000
#define REQUEST_EXIT 0x0001

class Terminal {
    public:
        struct termios t;
        int32_t requestDraw = 0;

        TerminalWindow frame;

        uint32_t width;
        uint32_t height;

        wchar_t* buffer;

        Terminal();

        uint32_t draw();
        void terminal_size_changed();
        void on_terminal_size_change(uint32_t width, uint32_t height);

        uint32_t handle_input();

        void create_window(TerminalWindow* parent, TerminalWindow window);

        void draw_window(TerminalWindow window, wchar_t* buffer, uint32_t offsetX, uint32_t offsetY);
};