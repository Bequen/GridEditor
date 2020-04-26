#pragma once

#include <cstdint>
#include "Editor/CameraController.h"
#include "Editor/Tiles/WindowTileInfo.h"

struct ViewportInfo {
    CameraController camera;
    WindowTileInfo tileInfo;
};