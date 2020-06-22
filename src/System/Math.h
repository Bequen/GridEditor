#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "Editor/Ray.h"

namespace Math {
    bool ray_box_intersection(const Ray* ray, glm::vec3 begin, glm::vec3 end);
};