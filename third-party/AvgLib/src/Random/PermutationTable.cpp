#include "avg/Random/PermutationTable.h"

#include <random>
#include <functional>

void PermutationTable::init(uint32_t seed) {
    size = 256;
    permutationTable = new uint32_t[size * 2];

    std::mt19937 generator(seed);
    std::uniform_real_distribution distribution;

    auto dice = std::bind(distribution, generator);
    float gradientLen2;
    for (unsigned i = 0; i < size; ++i) {
        permutationTable[i] = i;
    }

    std::uniform_int_distribution distributionInt;
    auto diceInt = std::bind(distributionInt, generator);

    // create permutation table
    for (unsigned i = 0; i < size; ++i)
        std::swap(permutationTable[i], permutationTable[diceInt() % size]);
    // extend the permutation table in the index range [256:512]
    for (unsigned i = 0; i < size; ++i) {
        permutationTable[size + i] = permutationTable[i];
    }
}

uint32_t& PermutationTable::operator[](int32_t index) {
    return permutationTable[index];
}