#pragma once

#include <cstdint>
#include <sys/stat.h>
#include <sys/types.h>
#include "StringLib.h"

#if __WIN32__ || __MINGW32__
    #define DIRECTORY_SEPARATOR '\\'
#elif __linux__
    #define DIRECTORY_SEPARATOR '/'
#endif

namespace avg
{
    /**
     * @brief  Namespace containing interface for working with files
     */
    namespace File
    {
        /**
         * @brief  Returns path to executable
         * @note   
         * @retval 
         */
        char* exe_path() {
            #if _WIN32
                char* result = new char[256];
                GetModuleFileName(NULL, result, 256);
                result[strlen(result)] = '\0';

                return result;
            #elif __linux__
                char* result = new char[256];
                //uint32_t length = readlink( "/proc/self/exe", result, 256);
                
                return result;
            #endif
        }
    };

    /**
     * @brief  Allows you to check wheter the file was changed
     * @note   You have to manage checking yourself
     */
    class FileWatcher
    {
        public:
            char *path;       // Path to the file that is being watched
            time_t lastCheck; // Last time file change was detected

            /**
             * @brief  Creates new FileWatcher instance
             * @param  path[]: Path to the file that will be watched
             */
            FileWatcher(char path[]);

            /**
             * @brief  Checks for file change time
             * @retval Returns true if file was changed from last check
             */
            bool check();
    };
}; 