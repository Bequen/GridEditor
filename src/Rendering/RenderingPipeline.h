#pragma once

#include <cstdint>

#include "System/Grid.h"

class RenderingPipeline {
    private:
        uint32_t voxel;

        uint32_t cameraBuffer;
        void* camera;

        Grid<int8_t>* grid;

    public:
        uint32_t shader;
        uint32_t boxShader;

        void init(Grid<int8_t>* grid);
        void update();
        void terminate();
};