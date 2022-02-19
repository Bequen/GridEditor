#include "avg/Random/VoronoiNoise.h"

#include <cstdint>
#include <cmath>
#include <random>
#include <functional>

#include "avg/Math/vector.h"
#include "avg/Debug.h"

void VoronoiNoise::init(uint32_t seed, uint32_t size) {
    this->size = size;
    cells = new avg::math::float2[size];

    std::mt19937 generator(seed); 
    std::uniform_real_distribution<float> distribution; 
    auto dice = std::bind(distribution, generator); 

    for(uint32_t i = 0; i < size; i++) {
        cells[i] = avg::math::float2(dice(), dice());
        permutationTable[i] = i;
    }

    std::uniform_int_distribution<int32_t> intDistr;
    auto diceInt = std::bind(intDistr, generator);
    for(uint32_t i = 0; i < size; i++) {
        uint32_t n = diceInt() % size;
        std::swap(permutationTable[n], permutationTable[i]);
        permutationTable[i + size] = permutationTable[i];
    }


    SUCCESS("Voronoi noise successfully initialized");
}

float VoronoiNoise::eval(avg::math::float2 position) {
    int32_t cell_x = (int)std::floor(position.x);
    int32_t cell_y = (int)std::floor(position.y);

    float pos_x = position.x - std::floor(position.x);
    float pos_y = position.y - std::floor(position.y);

    float min_d = 1.0f;
    avg::math::float2 point;

    for(int32_t x = -1; x <= 1; x++) {
        for(int32_t y = -1; y <= 1; y++) {
            avg::math::float2 point = cells[permutationTable[permutationTable[std::abs(cell_x + x) % 256] + std::abs(cell_y + y) % 256]];

            float distance = avg::vector::distance(point, position - avg::math::float2(cell_x, cell_y) - avg::math::float2(x, y));
            if(distance < min_d) {
                min_d = distance;
                //point = cells[permutationTable[permutationTable[std::abs((cell_x % 255)) + x] + std::abs((cell_y % 255) + y)]];
            }
        }
    }

    return min_d;
}