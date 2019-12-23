#include <cstdint>

#if _WIN32
  #include <windows.h>
#endif

#include <avg/Debug.h>

#include "System/Application.h"
#include "Editor/MainWindow.h"
#include "System/string256.h"
#include "System/ContentPipeline.h"

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