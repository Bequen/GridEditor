#pragma once

#include <cstdint>

#include "System/Grid.h"
#include "Quad.h"

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

        void init(Grid<int8_t>* grid);
        void update();
        void terminate();

        void solve_greedy_meshing(Quad**& quads, uint32_t*& counts);
        void greedy_meshing();
};