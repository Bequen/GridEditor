#include "VoxelGrid.h"

#include <glad/glad.h>

#include "Rendering/TextureLib.h"

VoxelGrid::VoxelGrid() : 
texture(0) {
    width = height = depth = 0;
} VoxelGrid::VoxelGrid(uint32_t size) {
    width = height = depth = size;
    texture = TextureLib::create_texture_3d(size, size, size, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr);
} VoxelGrid::VoxelGrid(uint32_t width, uint32_t height, uint32_t depth) :
width(width), height(height), depth(depth) {
    texture = TextureLib::create_texture_3d(width, height, depth, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr);
}