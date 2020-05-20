#pragma once

#include <cstdint>
#include "Editor/CameraController.h"
#include "Editor/Tiles/WindowTileInfo.h"

class Viewport;

struct ViewportInfo {
    Viewport* viewport;

    CameraController camera;
    WindowTileInfo tileInfo;
};