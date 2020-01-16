#pragma once

#include <cstdint>

#include "System/Grid.h"
#include "Quad.h"

/**
 * @brief  Where all the rendering is done
 * @note   
 * @retval None
 */
class RenderingPipeline {
    private:
        uint32_t voxel;

        uint32_t cameraBuffer;
        void* camera;

    public:
        Grid<int8_t>* grid;

        uint32_t shader;
        uint32_t boxShader;
        uint32_t skyShader;

        glm::vec4 skyColor;

        uint32_t topQuadVAO;
        uint32_t quadProgram;
        uint32_t polygonMode;

        /**
         * @brief  Initializes the rendering pipeline
         * @note   WARNING: >>WILL BE REFACTORED SOON<<
         * @param  grid: The grid that will be rendered
         * @retval None
         */
        void init(Grid<int8_t>* grid);

        /**
         * @brief  Update loop doing all the rendering
         * @note   Don't use this function, the Grid system is already doing it
         * @retval None
         */
        void update();

        /**
         * @brief  Terminates the rendering pipeline and cleans up after it
         * @note   
         * @retval None
         */
        void terminate();

        /**
         * @brief  Connects the set of quads
         * @note   You can use this on any set of quads divided into rows
         * @param  quads: Quads that will be merged
         * @param  counts: Number of quads in rows
         * @retval None
         */
        void solve_greedy_meshing(Quad**& quads, uint32_t*& counts);
};