#include <cstdint>

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include <stdio.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if _WIN32
  #include <windows.h>
#endif

#include <avg/Debug.h>

#include "System/Application.h"
#include "System/string256.h"
#include "System/ContentPipeline.h"

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

/**
  * @brief  Entry point for this application
  * @note   
  * @param  argc: Number of arguments
  * @param  argv: Stored arguments
  * @retval Returns 1 if there was an error, otherwise 0
  */
int32_t main(uint32_t argc, char** argv) {
    debug_init();

    Application app;
    app.init();
    app.update();
    app.terminate();

    return 0;
}