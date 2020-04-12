#pragma once

#include <cstdint>

#include "System/Voxels/Grid.h"
#include "Editor/Scene.h"
#include "Editor/RenderInfo.h"
#include "Framebuffer.h"

/**
 * @brief  Where all the rendering is done
 * @note   
 * @retval None
 */
class RenderingPipeline {
    private:
        uint32_t position;
        uint32_t positionIndex;
        uint32_t scale;

    public:
        glm::vec4 skyColor;
        uint32_t polygonMode;

        RenderInfo renderInfo;

        /**
         * @brief  Initializes the rendering pipeline
         * @note   WARNING: >>WILL BE REFACTORED SOON<<
         * @param  grid: The grid that will be rendered
         * @retval None
         */
        void init();

        /**
         * @brief  Update loop doing all the rendering
         * @note   Don't use this function, the Grid's system is already doing it
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
        void draw_grid(Grid grid, glm::mat4 model);
        void draw_sky(uint32_t mode);

        /**
         * @brief  Terminates the rendering pipeline and cleans up after it
         * @note   
         * @retval None
         */
        void terminate();
};