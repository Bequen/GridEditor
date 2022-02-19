#pragma once

#include <cstdint>
#include <glm/glm.hpp>

#include "System/Grid3D.h"
#include "Editor/VoxelGrid.h"
#include "System/Voxels/GridCache.h"
#include "Editor/RenderInfo.h"
#include "Editor/Enums.h"
#include "Editor/Viewport/Selection.h"
#include "Editor/Viewport/ExtrusionInfo.h"

#include "Editor/Scene/GridSize.h"
#include "Editor/Scene/SubGrid.h"
#include "Editor/Viewport/Actions/PaintAction.h"

struct SceneGrid {
private:
    SubGrid* subgrids;
    uint32_t subgridWidth;

    std::vector<Action> history;

    /* GridCache* cache;
    uint32_t cacheDepth;
    uint32_t cacheOffset;
    uint32_t cacheSize;
    uint32_t cacheIndex; */

public:
    // The user defined size, mostly differs from the actual grid size
    uint32_t width, depth, height;
    
    uint32_t subgridCount;

    SceneGrid();
    SceneGrid(uint32_t size);
    SceneGrid(uint32_t width, uint32_t depth, uint32_t height);

    void init_cache();

    void clear();
    void dup_buffer();

    void set(int32_t index, uint32_t value);
    void set(glm::vec3 position, uint32_t value);
    void set(int32_t x, int32_t y, int32_t z, uint32_t value);

    void
    set_subgrid(uint32_t subgrid, uint32_t index, uint8_t value);

    const int32_t get(uint32_t index) const;
    const int32_t get(glm::vec3 position) const;
    const int32_t get(uint32_t x, uint32_t y, uint32_t z) const;

    void fill(glm::vec3 start, glm::vec3 end, BrushMode brushMode, uint32_t value);
    void fill(uint32_t startX, uint32_t startY, uint32_t startZ,
                uint32_t endX, uint32_t endY, uint32_t endZ,
                BrushMode brushMode, 
                uint32_t value);

    void extrude(ExtrusionInfo* extrusion, int32_t offset, int32_t height, uint32_t value);

    void update_texture();

    bool intersects(glm::vec3 position);

    uint32_t texture_size(uint32_t width, uint32_t depth, uint32_t height);

    void resize(uint32_t width, uint32_t depth, uint32_t height);

    void update_cache();

    void undo();
    void redo();

    void draw(RenderInfo renderInfo) const;
    void draw_selected(RenderInfo renderInfo) const;

    void clear_selection();
    void
    unselect_all();
    void select_all();
    void select(glm::vec3 position);
    bool
    is_any_selected();

    bool
    is_selected(glm::vec3 position);

    void copy_buffer(SceneGrid* from);
    void* get_buffer(int32_t subgrid);
    
    BitGrid*
    get_grid(int32_t subgrid);

    SceneGrid& operator=(SceneGrid& grid);

    inline int32_t index(glm::vec3 position) {
        return std::floor(position.x) + std::floor(position.y) * width + std::floor(position.z) * (depth * width);
    }

private:
    inline int32_t get_subgrid(int32_t index) const {
        int32_t result = ((uint32_t)(index % width) / SUBGRID_SIZE + ((uint32_t)((index % (width * depth)) / width) / SUBGRID_SIZE) * (subgridWidth) + ((uint32_t)(index / (width * depth)) / SUBGRID_SIZE) * (subgridWidth * subgridWidth));
        if(result >= subgridCount)
            return -1;
        else
            return result;
    }

    inline int32_t get_subgrid(glm::vec3 position) const {
        int32_t result = ((uint32_t)position.x / SUBGRID_SIZE + ((uint32_t)position.y / SUBGRID_SIZE) * (subgridWidth) + ((uint32_t)position.z / SUBGRID_SIZE) * (subgridWidth * subgridWidth));
        if(result >= subgridCount)
            return -1;
        else
            return result;
    }
    inline int32_t get_subgrid(uint32_t x, uint32_t y, uint32_t z) const {
        int32_t result = (x / SUBGRID_SIZE + (y / SUBGRID_SIZE) * (subgridWidth) + (z / SUBGRID_SIZE) * (subgridWidth * subgridWidth));
        if(result >= subgridCount)
            return -1;
        else
            return result;
    }

    inline int32_t get_index(glm::vec3 position) const {
        int32_t result = (uint32_t)std::floor(position.x) % SUBGRID_SIZE + ((uint32_t)std::floor(position.y) % SUBGRID_SIZE) * SUBGRID_SIZE + ((uint32_t)std::floor(position.z) % SUBGRID_SIZE) * (SUBGRID_SIZE * SUBGRID_SIZE);
        if(result >= SUBGRID_SIZE * SUBGRID_SIZE * SUBGRID_SIZE)
            return -1;
        else
            return result;
    }
    inline int32_t get_index(uint32_t x, uint32_t y, uint32_t z) const {
        int32_t result = x % SUBGRID_SIZE + (y % SUBGRID_SIZE) * SUBGRID_SIZE + (z % SUBGRID_SIZE) * (SUBGRID_SIZE * SUBGRID_SIZE);
        if(result > SUBGRID_SIZE * SUBGRID_SIZE * SUBGRID_SIZE)
            return -1;
        else
            return result;
    }
    inline int32_t get_index(uint32_t index) const {
        glm::vec3 pos = glm::vec3(index % width, (index % (width * depth)) / depth, index / (width * depth));
        return get_index(pos);
    }
};
