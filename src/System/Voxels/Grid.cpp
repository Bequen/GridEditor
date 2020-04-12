#include "Grid.h"

#include <cstring>
#include <avg/Debug.h>
#include "Rendering/TextureLib.h"

Grid::Grid() : 
buffer(nullptr),
width(0), depth(0), height(0),
max(0), min(0) {
    // Constructor body
} 

Grid::Grid(uint32_t size) :
buffer(new int8_t[size * size * size]),
width(size), depth(size), height(size),
max(size * size * size), min(0) {
    // Constructor body
    memset(buffer, 0, width * depth * height);
    gridTexture = TextureLib::create_texture_3d(width, depth, height, buffer);
} 

Grid::Grid(uint32_t width, uint32_t depth, uint32_t height) :
buffer(new int8_t[width * depth * height]),
width(width), depth(depth), height(height),
max(width * depth * height), min(0) {
    // Constructor body
    memset(buffer, 0, width * depth * height);
    gridTexture = TextureLib::create_texture_3d(width, depth, height, buffer);
}


bool Grid::intersects(glm::vec3 point) {
    return point.x > 0 && point.x < width &&
        point.y > 0 && point.y < depth &&
        point.z > 0 && point.z < height;
}


int32_t Grid::get(uint32_t index) {
    if(index >= min && index < max) {
        return buffer[index];
    } return -1;
}  int32_t Grid::get(glm::vec3 pos) {
    if(pos.x < 0 || pos.x >= width ||
        pos.y < 0 || pos.y >= depth ||
        pos.z < 0 || pos.z >= height)
        return - 1;
    return buffer[(uint32_t)pos.x + (uint32_t)pos.y * width + (uint32_t)pos.z * (width * depth)];
} 

void Grid::set(uint32_t index, int8_t value) {
    if(index >= min && index < max) {
        buffer[index] = value;
    }
} void Grid::set(glm::vec3 pos, int8_t value) {
    if(pos.x < 0 || pos.x >= width ||
        pos.y < 0 || pos.y >= depth ||
        pos.z < 0 || pos.z >= height)
        return;

    assert_msg(buffer != nullptr, "Grid is not initialized");

    buffer[(uint32_t)pos.x + (uint32_t)pos.y * width + (uint32_t)pos.z * (width * depth)] = value;
}


void Grid::resize(uint32_t width, uint32_t depth, uint32_t height) {
    MESSAGE("Resizing grid");

    int8_t* tempBuffer = new int8_t[width * depth * height];
    memset(tempBuffer, 0, width * depth * height);
    memcpy(tempBuffer, buffer, this->width * this->depth * this->height);

    delete [] tempBuffer;
    buffer = tempBuffer;
    this->width = width; this->depth = depth; this->height = height;
    TextureLib::delete_texture(gridTexture);
    gridTexture = TextureLib::create_texture_3d(width, depth, height, buffer);
}