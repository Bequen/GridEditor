#pragma once

#include <cstdint>

struct PermutationTable {
    uint32_t size;
    uint32_t sizeMask;
    uint32_t* permutationTable;

    void init(uint32_t seed);

    uint32_t& operator[](int32_t index);
};