#include "SceneGrid.h"

#define CACHE_DEPTH 32

#include "Rendering/TextureLib.h"
#include "Rendering/RenderLib.h"

#include <avg/Debug.h>

#pragma region Constructors

SceneGrid::SceneGrid() :
SceneGrid(32, 32, 32) {

}

SceneGrid::SceneGrid(uint32_t size) :
SceneGrid(size, size, size) {

}

SceneGrid::SceneGrid(uint32_t width, uint32_t depth, uint32_t height) :
width(width), depth(depth), height(height) {
    uint32_t textureSize = texture_size(width, depth, height);
    MESSAGE("Texture size: " << textureSize << " " << width);
    subgridWidth = textureSize / SUBGRID_SIZE;
    subgridCount = subgridWidth * subgridWidth * subgridWidth;

    subgrids = new SubGrid[subgridCount];

    MESSAGE("Initializing " << subgridCount << " grids");
    for(uint32_t i = 0; i < subgridCount; i++) {
        subgrids[i] = SubGrid();
    }

    init_cache();
}

void SceneGrid::init_cache() {

}

#pragma endregion

void SceneGrid::clear() {
    /* for(int32_t i = 0; i < subgridCount; i++) {
        memset(grids[i].buffer, 0, SUBGRID_SIZE * SUBGRID_SIZE * SUBGRID_SIZE);
    } */
}

void SceneGrid::dup_buffer() {
    SubGrid* temp = new SubGrid[subgridCount];
    memcpy(temp, subgrids, subgridCount * sizeof(SubGrid));
    subgrids = temp;

    for(uint32_t i = 0; i < subgridCount; i++) {
        subgrids[i].dup_buffers();
    }
}

void
SceneGrid::set_subgrid(uint32_t subgrid, uint32_t index, uint8_t value) {
    subgrids[subgrid].set(index, value);
}

void SceneGrid::set(int32_t index, uint32_t value) {
    if(index > width * height * depth || index < 0)
        return;
    else {
        int32_t subgrid = get_subgrid(index);
        if(subgrid == -1)
            return;
        set_subgrid(subgrid, get_index(index), value);
    }
}

void SceneGrid::set(glm::vec3 position, uint32_t value) {
    //uint32_t index = std::floor(position.x) + std::floor(position.y) * grid.width + std::floor(position.z) * (grid.width * grid.depth);
    if(position.x >= width || position.y >= depth || position.z >= height ||
        position.x < 0.0 || position.y < 0.0f || position.z < 0.0f)
        return;

    int32_t subgrid = get_subgrid(position);
    int32_t index = get_index(position);
    if(index == -1 || subgrid == -1)
        return;

    set_subgrid(subgrid, index, value);
    //if(get(index) != value) {
        //if(cache[cacheOffset % cacheSize].count >= cache[cacheOffset % cacheSize].size) {
        //    cache[cacheOffset % cacheSize].resize(cache[cacheOffset % cacheSize].size + 256);
        //}
        //cache[cacheOffset % cacheSize].buffer[cache[cacheOffset % cacheSize].count++] = {index, grid.buffer[index], value};
        //grid.buffer[index] = value;
        //grid->buffer[index] = value;
    //}
}

void SceneGrid::set(int32_t x, int32_t y, int32_t z, uint32_t value) {
    if(x < 0 || y < 0 || z < 0) {
        return;
    }
    //uint32_t index = x + y * grid.width + z * (grid.width * grid.depth);

    //Grid3D<uint8_t>* grid = &subgrids[(x / SUBGRID_SIZE + (y / SUBGRID_SIZE) * (subgridWidth) + (z / SUBGRID_SIZE) * (subgridWidth * subgridWidth))].colors;
    uint32_t subgrid = (x / SUBGRID_SIZE + (y / SUBGRID_SIZE) * (subgridWidth) + (z / SUBGRID_SIZE) * (subgridWidth * subgridWidth));
    uint32_t index = x % SUBGRID_SIZE + (y % SUBGRID_SIZE) * SUBGRID_SIZE + (z % SUBGRID_SIZE) * (SUBGRID_SIZE * SUBGRID_SIZE);

    if(index >= width * height * depth)
        return;
    else {
        set_subgrid(subgrid, get_index(index), value);
        /* if(get(index) != value) {
            //if(cache[cacheOffset % cacheSize].count >= cache[cacheOffset % cacheSize].size) {
            //    cache[cacheOffset % cacheSize].resize(cache[cacheOffset % cacheSize].size + 256);
            //}
            //cache[cacheOffset % cacheSize].buffer[cache[cacheOffset % cacheSize].count++] = {index, grid.buffer[index], value};
            grid->buffer[index] = value;
        } */
    }
}


const int32_t SceneGrid::get(uint32_t index) const {
    assert_msg(subgrids, "Subgrids are not initialized");

    if(index >= width * height * depth)
        return -1;
    else {
        uint32_t subgrid = get_subgrid(index);
        uint32_t normalized = get_index(index);
        assert_msg(subgrids[subgrid].colors.buffer, "Subgrid buffer is not initialized");
        return subgrids[get_subgrid(index)].colors.buffer[get_index(index)];
    }
}

const int32_t SceneGrid::get(glm::vec3 position) const {
    if(position.x < 0 || position.x >= width ||
        position.y < 0 || position.y >= depth ||
        position.z < 0 || position.z >= height)
        return -1;
    
    Grid3D<uint8_t>* grid = &subgrids[((uint32_t)position.x / 8 + ((uint32_t)position.y / 8) * (subgridWidth) + ((uint32_t)position.z / 8) * (subgridWidth * subgridWidth))].colors;
    uint32_t index = (uint32_t)position.x % 8 + ((uint32_t)position.y % 8) * 8 + ((uint32_t)position.z % 8) * (8 * 8);

    //uint32_t index = std::floor(position.x) + std::floor(position.y) * width + std::floor(position.z) * (grid.width * grid.depth);
    return grid->buffer[index];
}

const int32_t SceneGrid::get(uint32_t x, uint32_t y, uint32_t z) const {
    /* uint32_t index = x + y * grid.width + z * (grid.width * grid.depth);
    if(index > grid.width * grid.height * grid.depth)
        return -1;
    else
        return grid.buffer[index]; */
    if(x < 0 || x > width ||
        y < 0 || y > depth ||
        z < 0 || z > height)
        return -1;
    
    Grid3D<uint8_t>* grid = &subgrids[((uint32_t)x / 8 + ((uint32_t)y / 8) * (subgridWidth) + ((uint32_t)z / 8) * (subgridWidth * subgridWidth))].colors;
    uint32_t index = (uint32_t)x % 8 + ((uint32_t)y % 8) * 8 + ((uint32_t)z % 8) * (8 * 8);

    //uint32_t index = std::floor(position.x) + std::floor(position.y) * width + std::floor(position.z) * (grid.width * grid.depth);
    return grid->buffer[index];
}


void SceneGrid::update_texture() {
    for(uint32_t i = 0; i < subgridCount; i++)
        TextureLib::update_texture_3d(subgrids[i].texture.texture, SUBGRID_SIZE, SUBGRID_SIZE, SUBGRID_SIZE, subgrids[i].colors.buffer);
}

bool SceneGrid::intersects(glm::vec3 position) {
    return  position.x < width && position.x > 0.0f &&
            position.y < depth && position.y > 0.0f &&
            position.z < height && position.z > 0.0f;
}


void SceneGrid::undo() {
    /* if(cacheDepth > cacheIndex) {
        MESSAGE("Undo " << cacheIndex << " for " << cache[cacheIndex].count);
        cacheIndex++;
        for(uint32_t i = 0; i < cache[(cacheOffset - cacheIndex) % cacheSize].count; i++) {
            set(cache[(cacheOffset - cacheIndex) % cacheSize].buffer[i].index, cache[(cacheOffset - cacheIndex) % cacheSize].buffer[i].color);
        }
    } */
}

void SceneGrid::redo() {
    /* if(cacheIndex > 0) {
        MESSAGE("Redo " << cacheIndex << " for " << cache[cacheIndex].count);
        for(uint32_t i = 0; i < cache[cacheDepth - cacheIndex].count; i++) {
            set(cache[cacheDepth - cacheIndex].buffer[i].index, cache[cacheDepth - cacheIndex].buffer[i].newColor);
        }
        cacheIndex--;
    } */
}

void SceneGrid::update_cache() {
    /* MESSAGE("Updating cache");

    // If user did undo, and then edit the buffer, we need to reset the cache
    if(cacheIndex > 0) {
        cacheDepth = -1;
        cacheIndex = 0;
    }

    cacheOffset++;

    // Do not extend the cache size
    cacheDepth = cacheDepth < cacheSize ? cacheDepth++ : cacheDepth;
    cache[cacheDepth % cacheSize].count = 0; */
}

void SceneGrid::resize(uint32_t width, uint32_t depth, uint32_t height) {
    // If the new size is smaller, we don't need to make new texture or compute anything
    /* if(this->width > width && this->depth > depth && this->height > height) {

    } else {
        if(voxelGrid.size > width && voxelGrid.size > depth && voxelGrid.size > height) {

        } else {
            voxelGrid.remove();
            voxelGrid = VoxelGrid(texture_size(width, depth, height));
        }
    }

    this->width = width;
    this->depth = depth;
    this->height = height;

    grid.resize(width, depth, height);
    update_texture(); */
}

uint32_t SceneGrid::texture_size(uint32_t width, uint32_t depth, uint32_t height) {
    uint32_t power = 2;
    for(; power < width && power < depth && power < height; ) { 
        power *= 2;
        ERROR(power << " " << width);
    }

    return power;
}

void SceneGrid::draw(RenderInfo renderInfo) const {
    glUseProgram(renderInfo.voxelProgram);
    uint32_t positionUniform = glGetUniformLocation(renderInfo.currentProgram, "index");
    uint32_t subgridUniform = glGetUniformLocation(renderInfo.currentProgram, "subgrid");
    uint32_t subgridLengthUniform = glGetUniformLocation(renderInfo.currentProgram, "subgridLength");
    uint32_t sizeUniform = glGetUniformLocation(renderInfo.currentProgram, "size");
    glUniform3ui(sizeUniform, SUBGRID_SIZE, SUBGRID_SIZE, SUBGRID_SIZE);
    glUniform1i(subgridLengthUniform, subgridWidth);

    // Draws subgrid
    for(uint32_t i = 0; i < subgridCount; i++) {
        glUniform1i(subgridUniform, i);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, subgrids[i].texture.texture);

        uint32_t streakBegin = 0;
        uint32_t streak = 0;

        for(uint32_t x = 0; x < SUBGRID_MAX; x++) {
            if(subgrids[i].is_set(x) > 0) {
                streak++;
            } else {
                if(streak > 0) {
                    glUniform1i(positionUniform, streakBegin);
                    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, streak);
                    streakBegin += streak; streak = 0;
                }
                streakBegin++;
            }
        }

        if(streak > 0) {
            glUniform1i(positionUniform, streakBegin);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, streak);
            streakBegin += streak; streak = 0;
        }
    }
}

void SceneGrid::draw_selected(RenderInfo renderInfo) const {
    glUseProgram(renderInfo.voxelSelectedProgram);
    uint32_t positionUniform = glGetUniformLocation(renderInfo.currentProgram, "index");
    uint32_t subgridUniform = glGetUniformLocation(renderInfo.currentProgram, "subgrid");
    uint32_t subgridLengthUniform = glGetUniformLocation(renderInfo.currentProgram, "subgridLength");
    uint32_t sizeUniform = glGetUniformLocation(renderInfo.currentProgram, "size");
    glUniform3ui(sizeUniform, 8, 8, 8);
    glUniform1i(subgridLengthUniform, subgridWidth);

    // Draws subgrid
    for(uint32_t i = 0; i < subgridCount; i++) {
        glUniform1i(subgridUniform, i);


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, subgrids[i].texture.texture);

        uint32_t streakBegin = 0;
        uint32_t streak = 0;

        Selection* selection = &subgrids[i].selection;
        BitGrid* grid = &subgrids[i].grid;

        for(uint32_t x = 0; x < SUBGRID_SIZE * SUBGRID_SIZE; x++) {
            for(uint32_t y = 0; y < 8; y++) {
                if(selection->grid.grid[x] & (1 << y) &&
                    grid->grid[x] & (1 << y)) {
                    streak++;
                } else {
                    if(streak > 0) {
                        glUniform1i(positionUniform, streakBegin);
                        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, streak);
                        streakBegin += streak; streak = 0;
                    }
                    streakBegin++;
                }
            }
        } 
        
        if(streak > 0) {
            glUniform1i(positionUniform, streakBegin);
            glDrawArraysInstanced(GL_TRIANGLES, 0, 36, streak);
            streakBegin += streak; streak = 0;
        }
    }
}

void SceneGrid::extrude(ExtrusionInfo* extrusion, int32_t offset, int32_t height, uint32_t value) {
    MESSAGE("Extruding by: " << height);
    if(height > 0) {
        for(uint32_t i = 0; i < extrusion->count; i++) {
            for(int32_t x = 0; x < height; x++) {
                set(extrusion->indices[i] + x * offset, value);
            }
        }
    } else {
        for(uint32_t i = 0; i < extrusion->count; i++) {
            for(int32_t x = 0; x > height; x--) {
                set(extrusion->indices[i] + x * offset, 0);
            }
        }
    }
}

void SceneGrid::clear_selection() {
    for(uint32_t i = 0; i < subgridCount; i++) {
        memset(subgrids[i].selection.grid.grid, 0, SUBGRID_SIZE * SUBGRID_SIZE * sizeof(GridRowType));
    }
}

void SceneGrid::select_all() {
    for(uint32_t i = 0; i < subgridCount; i++) {
        memcpy(subgrids[i].selection.grid.grid, subgrids[i].grid.grid, SUBGRID_SIZE * SUBGRID_SIZE * sizeof(GridRowType));
    }
}

void SceneGrid::unselect_all() {
    for(uint32_t i = 0; i < subgridCount; i++) {
        memset(subgrids[i].selection.grid.grid, 0, SUBGRID_SIZE * SUBGRID_SIZE * sizeof(GridRowType));
    }
}

void SceneGrid::select(glm::vec3 position) {
    if(position.x < 0.0f || position.y < 0.0f || position.z < 0.0f ||
    position.x >= width || position.y >= depth || position.z >= height) {
        return;
    }

    uint32_t subgrid = get_subgrid(position);
    subgrids[subgrid].select(get_index(position));
}

bool
SceneGrid::is_any_selected() {
    for(uint32_t i = 0; i < subgridCount; i++) {
        if(subgrids[i].is_any_selected()) {
            return true;
        }
    } return false;
}

bool
SceneGrid::is_selected(glm::vec3 position) {
    return subgrids[get_subgrid(position)].is_selected(get_index(position));
}

void SceneGrid::copy_buffer(SceneGrid* from) {
    for(uint32_t i = 0; i < subgridCount; i++) {
        memcpy(subgrids[i].colors.buffer, from->get_buffer(i), SUBGRID_SIZE * SUBGRID_SIZE * SUBGRID_SIZE * sizeof(int8_t));
        memcpy(subgrids[i].grid.grid, from->get_grid(i)->grid, SUBGRID_SIZE * SUBGRID_SIZE * sizeof(GridRowType));
    }
}

void* SceneGrid::get_buffer(int32_t subgrid) {
    if(subgrid >= subgridCount) {
        return nullptr;
    } else {
        return subgrids[subgrid].colors.buffer;
    }
}

BitGrid*
SceneGrid::get_grid(int32_t subgrid) {
    if(subgrid >= subgridCount) {
        return nullptr;
    } else {
        return &subgrids[subgrid].grid;
    }
}

SceneGrid& SceneGrid::operator=(SceneGrid& grid) {
    memcpy(this, &grid, sizeof(SceneGrid));
    return *this;
}

void SceneGrid::fill(glm::vec3 start, glm::vec3 end, BrushMode brushMode, uint32_t value) {
    if(start.x > end.x) { std::swap(start.x, end.x); }
    if(start.y > end.y) { std::swap(start.y, end.y); }
    if(start.z > end.z) { std::swap(start.z, end.z); }

    for(float x = std::floor(start.x); x <= std::floor(end.x); x++) {
        for(float y = std::floor(start.y); y <= std::floor(end.y); y++) {
            for(float z = std::floor(start.z); z <= std::floor(end.z); z++) {
                if(brushMode == BRUSH_MODE_ADD) {
                    set(glm::vec3(x, y, z), value);
                } else if(brushMode == BRUSH_MODE_PAINT) {
                    if(get(glm::vec3(x, y, z)) > 0)
                    set(glm::vec3(x, y, z), value);
                } else if(brushMode == BRUSH_MODE_SUBSTRACT) {
                    set(glm::vec3(x, y, z), 0);
                }
            }
        }
    } 
}
