#pragma once

#if __MINGW32__
    #include <stdio.h>
    #include <wchar.h>
    #include <windows.h>
#endif

#include <iostream>
#include <streambuf>
#include <cstdio>
#include <csignal>

#if __linux__
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif

#include "ASCII.h"

#define FOREGROUND_BLACK    "\033[0;30m"
#define FOREGROUND_RED      "\033[0;31m"
#define FOREGROUND_GREEN    "\033[0;32m"
#define FOREGROUND_YELLOW   "\033[0;33m"
#define FOREGROUND_BLUE     "\033[0;34m"
#define FOREGROUND_MAGENTA  "\033[0;35m"
#define FOREGROUND_CYAN     "\033[0;36m"
#define FOREGROUND_WHITE    "\033[0;37m"

#define BACKGROUND_BLACK    "\033[0;40m"
#define BACKGROUND_RED      "\033[0;41m"
#define BACKGROUND_GREEN    "\033[0;42m"
#define BACKGROUND_YELLOW   "\033[0;43m"
#define BACKGROUND_BLUE     "\033[0;44m"
#define BACKGROUND_MAGENTA  "\033[0;45m"
#define BACKGROUND_CYAN     "\033[0;46m"
#define BACKGROUND_WHITE    "\033[0;47m"

#define BOLD                "\033[1m"
#define RESET               "\033[0m"
#define INVERT              "\033[7m"
#define REINVERT            "\033[27m"

#define CURSOR_UP(n)        "\033[" << n << "A"
#define CURSOR_DOWN(n)      "\033[" << n << "B"
#define CURSOR_FORWARD(n)   "\033[" << n << "C"
#define CURSOR_BACK(n)      "\033[" << n << "D"
#define CURSOR_POSITION(x,y)"\033[" << y << ";" << x << "H"

#define CURSOR_SHOW         "\033[25h"
#define CURSOR_HIDE         "\033[25l"

#define ERROR(message) std::cout << FOREGROUND_RED << BOLD << INVERT << "[ERROR]" << RESET << FOREGROUND_RED << ": " << message << FOREGROUND_WHITE << std::endl;
#define WARNING(message) std::cout << FOREGROUND_YELLOW << BOLD << INVERT << "[WARNING]" << RESET << FOREGROUND_YELLOW << ": " << message << FOREGROUND_WHITE << std::endl;
#define MESSAGE(message) std::cout << FOREGROUND_BLUE << BOLD << INVERT << "[MESSAGE]" << RESET << FOREGROUND_BLUE << ": " << message << FOREGROUND_WHITE << std::endl;
#define SUCCESS(message) std::cout << FOREGROUND_GREEN << BOLD << INVERT << "[SUCCESS]" << RESET << FOREGROUND_GREEN << ": " << message << FOREGROUND_WHITE << std::endl;


#if DEBUG && __linux__
    #define ASSERT(exp) {                                                                                           \
        if(!(exp)) {                                                                                                \
            ERROR("Assertion failed in " << __FILE__ << " at " << __func__ << " on line " << __LINE__);             \
            raise(SIGTRAP);                                                                                         \
        }                                                                                                           \
    } 
    #define ASSERT_MSG(exp, msg) {                                                                                  \
        if(!(exp)) {                                                                                                \
            ERROR("Assertion failed in " << __FILE__ << " at " << __func__ << " on line " << __LINE__ << "\n" <<    \
            msg);                                                                                               \
            raise(SIGTRAP);                                                                                         \
        }                                                                                                           \
    }
#elif (defined(DEBUG) && defined(__win32__)) 
    #define ASSERT(exp)             assert(exp)
    #define ASSERT_MSG(exp, msg)    assert(exp)
#else
    #define ASSERT(exp)             ((void)(exp))
    #define ASSERT(exp, msg)        ((void)(exp))
#endif

#if !defined(assert)
    #define assert(exp)             ASSERT(exp)
#endif

#if !defined(assert_msg)
    #define assert_msg(exp, msg)             ASSERT_MSG(exp, msg)
#endif



/**
 * @brief  There might be some things to get ready
 * @note   It allows default window's console to display colors
 * @retval None
 */
inline void debug_init() {
    #if __MINGW32__
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE)
            std::cout << "Couldn't initialize Debug Interface. Invalid output handle!" << std::endl;
        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode))
            std::cout << "Unable to get console mode" << std::endl;

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, dwMode))
            std::cout << "Unable to set console mode" << std::endl;
    #endif
}