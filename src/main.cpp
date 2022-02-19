#include <cstdint>
#include <avg/Debug.h>

#include "System/Application.h"

/**
  * @brief  Entry point for this application
  * @note   
  * @param  argc: Number of arguments
  * @param  argv: Stored arguments
  * @retval Returns 1 if there was an error, otherwise 0
  */
int32_t main(int32_t argc, char** argv) {
    MESSAGE("Starting up");
    debug_init();

    /* Entire application life span is right here */
    Application app;
    app.init();
    app.update();
    app.terminate();

    WARNING("Stopping");

    return 0;
}

