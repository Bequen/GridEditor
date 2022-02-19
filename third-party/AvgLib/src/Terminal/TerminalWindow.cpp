#include "avg/Terminal/TerminalWindow.h"

TerminalWindow::TerminalWindow()  :
buffer(nullptr), width(0), height(0), padding(0), name(nullptr), children(nullptr), childCount(0) {

}

TerminalWindow::TerminalWindow(char* name, int32_t width, int32_t height, int32_t padding) :
buffer(new char[1024]), width(width), height(height), padding(padding), name(name), children(nullptr), childCount(0) {

}


void TerminalWindow::set_size(uint32_t width, uint32_t height)  {
    this->width = width;
    this->height = height;
}