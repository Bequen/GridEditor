#pragma once

#include <cstdint>

#include "System/Grid.h"
#include "Quad.h"
#include "System/Scene.h"

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
        uint32_t scale;

    public:
        uint32_t shader;
        uint32_t boxShader;
        uint32_t skyShader;

        glm::vec4 skyColor;

        uint32_t quadVAO;
        uint32_t quadProgram;
        uint32_t polygonMode;

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
        void draw_scene(Framebuffer framebuffer, Scene* scene, glm::vec3 view);
        void draw_grid(Grid<int8_t> grid, glm::vec3 view);
        void draw_sky();

        void merge_quad(_QuadMesh& quadMesh, _Quad quad, uint32_t& quadIndex, uint32_t index);

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
        void solve_greedy_meshing(Quad**& quads, uint32_t*& counts, uint32_t size);
};