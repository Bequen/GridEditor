#pragma once

#include <cstdint>

#include "Rendering/Light.h"
#include "Entity.h"

struct LightEntity {
    Entity entity;
    Light* light;
};