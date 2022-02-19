#include "avg/Random/PerlinNoise.h"

#include <random>
#include <functional>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <ctime>

#include "avg/Math/Math.h"
#include "avg/Math/vector.h"

void avg::PerlinNoise::init(PermutationTable permutationTable) { 
    this->size = 256; 
    this->sizeMask = 255;
    this->gradients = new avg::math::float3[size];
    this->permutationTable = permutationTable;

    seed = std::rand();

    std::mt19937 generator(seed);
    std::uniform_real_distribution distribution;
    auto dice = std::bind(distribution, generator);
    float gradientLen2;
    for (unsigned i = 0; i < size; ++i) {
        do {
            gradients[i] = avg::math::float3(2 * dice() - 1, 2 * dice() - 1, 2 * dice() - 1);
            gradientLen2 = avg::vector::length2(gradients[i]);
        } while (gradientLen2 > 1);
        gradients[i] = avg::vector::normalize(gradients[i]); // normalize gradient
    }
}

void avg::PerlinNoise::init(PermutationTable permutationTable, uint32_t seed) {
    this->seed = seed; 
    this->gradients = new avg::math::float3[256]; 
    this->size = 256;
    this->sizeMask = 255;
    this->permutationTable = permutationTable;

    std::mt19937 generator(seed);
    std::uniform_real_distribution distribution;
    auto dice = std::bind(distribution, generator);
    float gradientLen2;
    for (unsigned i = 0; i < size; ++i) {
        do {
            gradients[i] = avg::math::float3(2 * dice() - 1, 2 * dice() - 1, 2 * dice() - 1);
            gradientLen2 = avg::vector::length2(gradients[i]);
        } while (gradientLen2 > 1);
        gradients[i] = avg::vector::normalize(gradients[i]); // normalize gradient
    }

    /* std::uniform_int_distribution distributionInt;
    auto diceInt = std::bind(distributionInt, generator);
    // create permutation table
    for (unsigned i = 0; i < size; ++i)
        std::swap(permutationTable[i], permutationTable[diceInt() & sizeMask]);
    // extend the permutation table in the index range [256:512]
    for (unsigned i = 0; i < size; ++i) {
        permutationTable[size + i] = permutationTable[i];
    } */
}

int32_t avg::PerlinNoise::hash(int32_t x, int32_t y, int32_t z) {
    return permutationTable[permutationTable[permutationTable[x] + y] + z];
}

float avg::PerlinNoise::smoothstep(float t) {
    //return a * a * (3 - 2 * a);
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float avg::PerlinNoise::lerp(float a, float b, float t) {
    return a * (1 - t) + b * t;
}

float avg::PerlinNoise::eval(float x, float y, float z) {
        int xi0 = ((int)std::floor(x)) & sizeMask; 
        int yi0 = ((int)std::floor(y)) & sizeMask; 
        int zi0 = ((int)std::floor(z)) & sizeMask; 
 
        int xi1 = (xi0 + 1) & sizeMask; 
        int yi1 = (yi0 + 1) & sizeMask; 
        int zi1 = (zi0 + 1) & sizeMask; 
 
        float tx = x - ((int)std::floor(x)); 
        float ty = y - ((int)std::floor(y)); 
        float tz = z - ((int)std::floor(z)); 
 
        float u = smoothstep(tx); 
        float v = smoothstep(ty); 
        float w = smoothstep(tz); 
 
        // gradients at the corner of the cell
        const avg::math::float3 &c000 = gradients[hash(xi0, yi0, zi0)]; 
        const avg::math::float3 &c100 = gradients[hash(xi1, yi0, zi0)]; 
        const avg::math::float3 &c010 = gradients[hash(xi0, yi1, zi0)]; 
        const avg::math::float3 &c110 = gradients[hash(xi1, yi1, zi0)]; 
 
        const avg::math::float3 &c001 = gradients[hash(xi0, yi0, zi1)]; 
        const avg::math::float3 &c101 = gradients[hash(xi1, yi0, zi1)]; 
        const avg::math::float3 &c011 = gradients[hash(xi0, yi1, zi1)]; 
        const avg::math::float3 &c111 = gradients[hash(xi1, yi1, zi1)]; 
 
        // generate vectors going from the grid points to p
        float x0 = tx, x1 = tx - 1; 
        float y0 = ty, y1 = ty - 1; 
        float z0 = tz, z1 = tz - 1; 
 
        avg::math::float3 p000 = avg::math::float3(x0, y0, z0); 
        avg::math::float3 p100 = avg::math::float3(x1, y0, z0); 
        avg::math::float3 p010 = avg::math::float3(x0, y1, z0); 
        avg::math::float3 p110 = avg::math::float3(x1, y1, z0); 
 
        avg::math::float3 p001 = avg::math::float3(x0, y0, z1); 
        avg::math::float3 p101 = avg::math::float3(x1, y0, z1); 
        avg::math::float3 p011 = avg::math::float3(x0, y1, z1); 
        avg::math::float3 p111 = avg::math::float3(x1, y1, z1); 
 
        // linear interpolation
        float a = lerp(avg::vector::dot(c000, p000), avg::vector::dot(c100, p100), u); 
        float b = lerp(avg::vector::dot(c010, p010), avg::vector::dot(c110, p110), u); 
        float c = lerp(avg::vector::dot(c001, p001), avg::vector::dot(c101, p101), u); 
        float d = lerp(avg::vector::dot(c011, p011), avg::vector::dot(c111, p111), u); 
 
        float e = lerp(a, b, v); 
        float f = lerp(c, d, v); 
 
        return lerp(e, f, w) + 0.5f;  
}