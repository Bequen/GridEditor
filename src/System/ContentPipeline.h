#pragma once

#include <cstdint>

#include "string256.h"
#include "Editor/Scene.h"

namespace ContentPipeline {
    string256 asset_path(char* dir, char* name, char* extension);

    char* load_text(char* path, uint32_t* length);

    void save_scene(Scene scene, char* path);
    Scene load_scene(char* path);

    #define GRID_FILE_GRID          0x0001
    #define GRID_FILE_LIGHT         0x0002
    #define GRID_FILE_PARTICLES     0x0003

    void save_grid(Scene scene, char* path);
    Scene load_grid(char* path);
};