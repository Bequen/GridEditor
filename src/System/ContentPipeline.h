#pragma once

#include <cstdint>

#include "string256.h"
#include "Scene.h"

namespace ContentPipeline {
    string256 asset_path(char* dir, char* name, char* extension);

    char* load_text(char* path, uint32_t* length);

    void save_scene(Scene scene, char* path);
    Scene load_scene(char* path);
};