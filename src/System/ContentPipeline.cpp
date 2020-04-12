#include "ContentPipeline.h"

#include <iostream>
#include <fstream>
#include <cstring>

#include <avg/Debug.h>

#define FORMAT_RGB      0x0001
#define FORMAT_RGBA     0x0002

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
    /* std::ofstream stream = std::ofstream(path, std::ios::binary | std::ios::out);

    uint32_t value = 0;
    stream.write((char*)&value, 4);
    stream.write((char*)&value, 4);
    stream.write((char*)&value, 4);

    stream.write((char*)scene.palette, 256 * sizeof(RGB32));

    stream.write((char*)&scene.gridsCount, 4);
    for(uint32_t i = 0; i < scene.gridsCount; i++) {
        Grid grid = scene._grids[i];
        stream.write((char*)&grid.width, 4);
        stream.write((char*)&grid.depth, 4);
        stream.write((char*)&grid.height, 4);
        stream.write((char*)grid.buffer, grid.width * grid.depth * grid.height);
        stream.write("\0", 1);
    }

    stream.close(); */
}

Scene ContentPipeline::load_grid(char* path) {
    /* Scene result;
    
    std::ifstream stream = std::ifstream(path, std::ios::binary | std::ios::in);

    uint32_t paletteSize, paletteDepth, paletteFormat;
    stream.read((char*)&paletteSize, 4);
    stream.read((char*)&paletteDepth, 4);
    stream.read((char*)&paletteFormat, 4);

    RGB32* palette = new RGB32[256];
    stream.read((char*)palette, 256 * sizeof(RGB32));

    uint32_t gridCount = 0;
    stream.read((char*)&gridCount, 4);
    MESSAGE("Loading " << gridCount << " grids");
    result.init(gridCount);
    memcpy(result.palette, palette, 256 * sizeof(RGB32));
    delete [] palette;

    for(uint32_t i = 0; i < gridCount; i++) {
        Grid grid;
        uint32_t width, depth, height;

        stream.read((char*)&width, 4);
        stream.read((char*)&depth, 4);
        stream.read((char*)&height, 4);

        grid = Grid(width, depth, height);
        if(grid.buffer == nullptr)
            grid.buffer = new int8_t[width * depth * height];

        stream.read((char*)grid.buffer, width * depth * height);
        char c;
        stream.read(&c, 1);
        result.add_grid(grid, "1");
    }
    stream.close();

    return result; */
}