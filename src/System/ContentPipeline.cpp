#include "ContentPipeline.h"

#include <iostream>
#include <fstream>
#include <cstring>

#include <avg/Debug.h>

string256 ContentPipeline::asset_path(char* dir, char* name, char* extension) {
    string256 path;
    uint32_t length = readlink("/proc/self/exe", path.str, 256);
    path.length += length - strlen(PROJECT_NAME);

    path = string256_concat(path, "assets/");
    path = string256_concat(path, dir);
    path = string256_concat(path, name);
    path = string256_concat(path, extension);

    return path;
}

char* ContentPipeline::load_text(char* path, uint32_t* length) {
    std::ifstream stream = std::ifstream(path, std::ios::ate | std::ios::binary);

    if(!stream.is_open()) {
        ERROR("Couldn't open file!");
        return nullptr;
    }

    uint32_t textLength = stream.tellg();
    stream.seekg(0);

    char* result = new char[textLength + 1];
    stream.read(result, textLength);
    result[textLength] = '\0';

    if(length)
        *length = textLength;

    return result;
}