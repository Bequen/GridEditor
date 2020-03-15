#pragma once

#include <cstdint>

#include "System/Voxels/Grid.h"
#include "System/Scene.h"
#include "Editor/Tiles/PerformanceMonitor.h"

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

        uint32_t position;
        uint32_t positionIndex;
        uint32_t scale;

    public:
        uint32_t voxelProgram;
        uint32_t boxShader;
        uint32_t skyShader;

        glm::vec4 skyColor;

        uint32_t quadVAO;
        uint32_t quadProgram;
        uint32_t polygonMode;

        PerformanceStat* perfStat;

        /**
         * @brief  Initializes the rendering pipeline
         * @note   WARNING: >>WILL BE REFACTORED SOON<<
         * @param  grid: The grid that will be rendered
         * @retval None
         */
        void init();

        /**
         * @brief  Update loop doing all the rendering
         * @note   Don't use this function, the Grid system is already doing it
         * @retval None
         */
        void update();

        /**
         * @brief  Draws the scene into specific buffer
         * @note   
         * @param  framebuffer: The framebuffer in which we will be rendering the scene
         * @param  scene: Scene that will be rendered
         * @param  view: Position of the viewing camera
         * @retval None
         */
        void draw_scene(Framebuffer framebuffer, Scene* scene);
        void draw_grid(_Grid grid);
        void draw_sky();

        /**
         * @brief  Terminates the rendering pipeline and cleans up after it
         * @note   
         * @retval None
         */
        void terminate();
};