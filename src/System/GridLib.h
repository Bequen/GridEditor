#pragma once

#include <cstdint>

#include "Rendering/Quad.h"
#include "Rendering/QuadBuffer.h"
#include "Rendering/QuadMesh.h"
#include "Grid.h"

namespace GridLib {
    /**
     * @brief  Meshing method for voxel styled meshes
     * @note   This method bets on lowest possible number of polygon by merging voxels next to each other into one.
     * @param  grid: Grid that will be meshed
     * @retval Returns actual quads for rendering
     */
    QuadMesh greedy_meshing(Grid<int8_t> grid);

    void dynamic_greedy_meshing(QuadMesh* quadMesh, Grid<int8_t> grid, glm::vec3 start, glm::vec3 end);

    void greedy_meshing_merging(Quad**& quads, uint32_t*& counts, uint32_t size);

    /**
     * @brief  Meshing method for voxel styled meshes
     * @note   This method simply removes faces that are facing some other voxel. It can be particuraly useful when you need 1:1 quads for AO.
     * @param  grid: Grid that will be meshed
     * @retval Returns actual quads for rendering
     */
    QuadMesh doubles_remove_meshing(Grid<int8_t> grid);

    int32_t* get_voxel(uint32_t p, uint32_t x, uint32_t y, uint32_t z);
};