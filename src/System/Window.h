#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WINDOW_SIZE_WINDOWED 0x0000
#define WINDOW_SIZE_FULLSCREEN 0x0001
#define WINDOW_SIZE_BORDERLESS_FULLSCREEN 0x0002

class Window {
    public:
        GLFWwindow* window;
        int32_t width;
        int32_t height;

        void init();
        void init(int32_t width, int32_t height);
        void init(char* name, int32_t width, int32_t height);

        void update();
        void terminate();

        void change_size(uint32_t size);
        void change_size(uint32_t width, uint32_t height);

        bool should_close();
};