#pragma once

#include <cstdint>

struct Component {
    uint32_t id;
    uint32_t type;

    void* data;
};