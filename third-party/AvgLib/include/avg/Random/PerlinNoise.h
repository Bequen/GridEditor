#pragma once

#include "avg/Math/float3.h"
#include <cstdint>
#include "PermutationTable.h"

namespace avg {
    class PerlinNoise {
        public:
            uint32_t seed; // Used seed, useful to create same map

            uint32_t size; // Size of the map, default value is 256
            uint32_t sizeMask; // Mask, default value is @param:size - 1 

            PermutationTable permutationTable;

            avg::math::float3* gradients; // Actual table with gradients

            /**
             * @brief  Default constructor
             * @note   Gives all the values their default state
             */
            void init(PermutationTable permutationTable);

            /**
             * @brief  Constructor that allows specific seed
             * @note   
             * @param  seed: Seed that will be used for generator
             * @retval 
             */
            void init(PermutationTable permutationTable, uint32_t seed);

            /**
             * @brief  Evaluate noise at specific position
             * @note   3 dimensional
             * @param  x: X Component of the position
             * @param  y: Y Component of the position
             * @param  z: Z Component of the position
             * @retval return value
             */
            float eval(float x, float y, float z);

            /**
             * @brief  Smoothstep function
             * @note   Ken Perlin's implementation
             * @param  a: value that will be smoothed
             * @retval returns smoothed value
             */
            float smoothstep(float a);
            int32_t hash(int32_t x, int32_t y, int32_t z);
            float lerp(float a, float b, float t);
    };
};