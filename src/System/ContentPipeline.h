#pragma once

#include <cstdint>

#include "string256.h"

namespace ContentPipeline {
    string256 asset_path(char* dir, char* name, char* extension);

    char* load_text(char* path, uint32_t* length);
};