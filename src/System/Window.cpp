#include "Window.h" 
#include <avg/Debug.h>

void Window::init() {
    MESSAGE("Creating default window");

    width = 720;
    height = 480;

    if (!glfwInit())
        ERROR("Couldn't initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, PROJECT_NAME, 0, 0);

    if (!window) {
        ERROR("GLFW Window creation failed!");

        const char *message;
        uint32_t error = glfwGetError(&message);
        ERROR(message);

        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        ERROR("Failed to initialize GLAD");

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);
}

void Window::init(int32_t width, int32_t height) {
    MESSAGE("Creating window with size " << width << "x" << height);

    this->width = width;
    this->height = height;

    if (!glfwInit())
        ERROR("Couldn't initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, PROJECT_NAME, 0, 0);

    if (!window) {
        ERROR("GLFW Window creation failed!");

        const char *message;
        uint32_t error = glfwGetError(&message);
        ERROR(message);

        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        ERROR("Failed to initialize GLAD");

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);
}

void Window::init(char* name, int32_t width, int32_t height) {
    MESSAGE("Creating window with size " << width << "x" << height);

    this->width = width;
    this->height = height;

    if (!glfwInit())
        ERROR("Couldn't initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, name, 0, 0);

    if (!window) {
        ERROR("GLFW Window creation failed!");

        const char *message;
        uint32_t error = glfwGetError(&message);
        ERROR(message);

        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        ERROR("Failed to initialize GLAD");

    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
}

void Window::change_size(uint32_t size) {
    switch (size) {
        case WINDOW_SIZE_WINDOWED: {
            MESSAGE("Changing the window size to windowed");
            glfwSetWindowMonitor(window, nullptr, 0, 0, width, height, 1);
            break;
        } case WINDOW_SIZE_FULLSCREEN: {
            MESSAGE("Changing the window size to fullscreen");
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            glfwGetMonitorPhysicalSize(monitor, &width, &height);

            glfwSetWindowMonitor(window, monitor, 0, 0, width, height, 1);
            break;
        } default: {
            ERROR("Invalid window size");
            break;
        }
    }
}

void Window::set_cursor_pos(float x, float y) {
    glfwSetCursorPos(window, x, y);
}

void Window::update() {
    glfwPollEvents(); // Try switching with glfwWaitEvents();
    glfwSwapBuffers(window);
}

void Window::terminate() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::should_close() {
    return glfwWindowShouldClose(window);
}

bool Window::is_key_down(uint32_t key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool Window::is_mouse_button_down(uint32_t button) {
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

glm::vec3 Window::get_normalized_cursor_pos() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);

    glm::vec3 result = glm::vec3((2.0f * (float)x) / width - 1.0f, 1.0f - (2.0f * (float)y) / height, 1.0f);

    return result;
}