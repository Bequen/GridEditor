#include "ContentPipeline.h"

#include <iostream>
#include <cstring>

#include <avg/Debug.h>

#define FORMAT_RGB      0x0001
#define FORMAT_RGBA     0x0002

#define GRID_FILE_CATEGORY_GRIDS        0x01
#define GRID_FILE_CATEGORY_SCENE_GRAPH  0x02
#define GRID_FILE_CATEGORY_LIGHTS       0x03

#define GRID_FILE_VALUE_SIZE            0x01
#define GRID_FILE_VALUE_CATEGORY_SIZE   0x02
#define GRID_FILE_VALUE_CATEGORY_COUNT  0x03
#define GRID_FILE_VALUE_GRID            0x04
#define GRID_FILE_VALUE_VALUE_COUNT     0x05
#define GRID_FILE_VALUE_OBJECT_ID       0x06
#define GRID_FILE_VALUE_CHILD_COUNT     0x07
#define GRID_FILE_VALUE_CHILDREN        0x08
#define GRID_FILE_VALUE_OBJECT_TYPE     0x09
#define GRID_FILE_VALUE_DATA_OFFSET     0x0a
#define GRID_FILE_VALUE_NAME            0x0b

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

void ContentPipeline::save_grid(Scene scene, char* path) {
}

Scene ContentPipeline::load_grid(char* path) {

}

