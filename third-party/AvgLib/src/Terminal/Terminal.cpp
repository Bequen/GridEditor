#include "avg/Terminal/Terminal.h"

Terminal::Terminal() {
    std::locale::global (std::locale ("en_US.UTF-8"));
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    buffer = new wchar_t[0];
    terminal_size_changed();

    frame.padding = 1;

    frame.children = new TerminalWindow[5];
    frame.childCount = 0;
    frame.children[frame.childCount++] = TerminalWindow(nullptr, 10, 10, 1);
    frame.children[frame.childCount++] = TerminalWindow(nullptr, 30, 10, 1);
}

uint32_t Terminal::draw() {
    terminal_size_changed();

    if(requestDraw) {
        draw_window(frame, buffer, 0, 0);
        requestDraw = 0;

        std::wcout << buffer;
        std::flush(std::wcout);
    }

    return handle_input();
}

uint32_t Terminal::handle_input() {
    char input = std::getchar();

    // If arrow is pressed
    if(input == '\033') {
        std::getchar();
        switch(std::getchar()) {

        }

        return NO_MESSAGE;
    } else if(input == 'q') {
        return REQUEST_EXIT;
    } else {
        return NO_MESSAGE;
    }
}

void Terminal::terminal_size_changed() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    if(w.ws_row != frame.height || w.ws_col != frame.width) {
        on_terminal_size_change(w.ws_col, w.ws_row);
        requestDraw = true;
    }
}

void Terminal::on_terminal_size_change(uint32_t width, uint32_t height)  {
    this->frame.set_size(width, height);
    delete [] buffer;
    buffer = new wchar_t[width * height];
}

void Terminal::create_window(TerminalWindow* parent, TerminalWindow window)  {
    parent->children[parent->childCount++] = window;
    requestDraw = 1;
}

void Terminal::draw_window(TerminalWindow window, wchar_t* buffer, uint32_t offsetX, uint32_t offsetY)  {
    uint32_t oX = offsetX;
    uint32_t oY = offsetY;

    buffer[offsetX + frame.width * offsetY] = LEFT_TOP;
    int32_t passed = 0;

    for(uint32_t x = 0; x < window.width - 2; x++) {
        if(passed)
            buffer[++offsetX + frame.width * offsetY] = ROOF;
        else {
            if(!window.name || window.name[x] == '\0') {
                passed = 1;
                buffer[++offsetX + frame.width * offsetY] = ROOF;
                continue;
            }
            buffer[++offsetX + frame.width * offsetY] = window.name[x];
        }
    } 
    buffer[++offsetX + frame.width * offsetY] = RIGHT_TOP;

    
    for(uint32_t y = 0; y < window.height - 2; y++) {
        offsetX = oX;
        buffer[offsetX + frame.width * ++offsetY] = WALL;
        for(uint32_t x = 0; x < window.width - 2; x++)
            buffer[++offsetX + frame.width * offsetY] = (wchar_t)' ';
        buffer[++offsetX + frame.width * offsetY] = WALL;
    } offsetX = oX;
    
    buffer[offsetX + frame.width * offsetY] = LEFT_BOTTOM;
    for(uint32_t x = 0; x < window.width - 2; x++) {
        buffer[++offsetX + frame.width * offsetY] = ROOF;
    } buffer[++offsetX + frame.width * offsetY] = RIGHT_BOTTOM;

    for(uint32_t i = 0; i < window.childCount; i++) {
        
        draw_window(window.children[i], buffer, oX, oY);
        
        if(window.flow == FLOW_HORIZONTAL) {
            oX += window.children[i].width - 1;
        }
    }
}