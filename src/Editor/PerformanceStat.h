#pragma once

#include <cstdint>

struct PerformanceStat {
    uint32_t id;
    char* name;

    float* time;
    uint32_t bufferSize;

    uint32_t count;
    double sum;

    uint32_t index;
};