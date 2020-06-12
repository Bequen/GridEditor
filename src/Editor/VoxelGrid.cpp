#include "VoxelGrid.h"

#include <glad/glad.h>
#include <avg/Debug.h>

#include "Rendering/TextureLib.h"

VoxelGrid::VoxelGrid() : 
texture(0), size(0) {
    
} VoxelGrid::VoxelGrid(uint32_t size) :
size(size) {
    texture = TextureLib::create_texture_3d(size, size, size, GL_R8, GL_RED, GL_UNSIGNED_BYTE, nullptr);
}

void VoxelGrid::remove() {
    TextureLib::delete_texture(texture);
}