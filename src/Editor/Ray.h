#pragma once

#include <cstdint>

#include "System/Window.h"
#include "Cursor.h"

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
};