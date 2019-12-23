#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define WINDOW_SIZE_WINDOWED 0x0000
#define WINDOW_SIZE_FULLSCREEN 0x0001
#define WINDOW_SIZE_BORDERLESS_FULLSCREEN 0x0002

class Window {
    private:
        GLFWwindow* window;

    public:
        int32_t width;
        int32_t height;

        void init();
        void init(int32_t width, int32_t height);
        void init(char* name, int32_t width, int32_t height);

        void update();
        void terminate();

        void change_size(uint32_t size);
        void change_size(uint32_t width, uint32_t height);

        void set_cursor_pos(float x, float y);

        bool should_close();

        bool is_mouse_button_down(uint32_t button);
        bool is_key_down(uint32_t key);
        glm::vec3 get_normalized_cursor_pos();
};