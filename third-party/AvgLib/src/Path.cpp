#include "avg/Path.h"

#include "avg/StringLib.h"
#include <stdlib.h>
#include <stdio.h>

// Go
void avg::Path::go_forward(char* buffer, char* addition) {
    uint32_t length = avg::StringLib::length(buffer);
    memcpy(buffer + length, addition, avg::StringLib::length(addition));
}

void avg::Path::go_backward(char* buffer) {
    uint32_t length = avg::StringLib::length(buffer);
    for(uint32_t i = length; i > 0; i--) {
        if(buffer[i] == '/') {
            buffer[i] = '\0';
            break;
        }
    }
}


// Gets
char* avg::Path::get_file(char* buffer) {
    uint32_t length = avg::StringLib::length(buffer);
    char* result;
    for(uint32_t i = length; i > 0; i--) {
        if(buffer[i] == '/') {
            result = buffer + i + 1;
            break;
        }
    }

    return result;
}

char* avg::Path::get_file_extension(char* buffer) {
    uint32_t length = avg::StringLib::length(buffer);
    char* result;
    for(uint32_t i = length; i > 0; i--) {
        if(buffer[i] == '.') {
            result = buffer + i;
            break;
        }
    }

    return result;
}

char* avg::Path::get_path(char* buffer) {
    uint32_t length = avg::StringLib::length(buffer);
    char* result;
    for(uint32_t i = length; i > 0; i--) {
        if(buffer[i] == '/') {
            result = (char*)malloc(i);
            memcpy(result, buffer, i);
            result[i] = '\0';
            break;
        }
    }

    return result;
}