#pragma once

#include <cstdint>

struct ivec3 {
    int32_t x, y, z;

    ivec3() :
    x(0), y(0), z(0) {

    } ivec3(int32_t value) :
    x(value), y(value), z(value) {

    } ivec3(int32_t x, int32_t y, int32_t z) :
    x(x), y(y), z(z) {

    }
};

struct VoxelOctreeNode {
    VoxelOctreeNode* children;
    uint32_t value;

    uint32_t size;

    void set(ivec3 pos, uint32_t value, uint32_t desiredLevel, uint32_t level) {
        if(desiredLevel == level) {
            this->value = value; return;
        }

        if(children == nullptr) {
            children = new VoxelOctreeNode[8];
        }

        uint32_t x = pos.x / size;
        uint32_t y = pos.y / size;
        uint32_t z = pos.z / size;

        ivec3 newPos = ivec3(pos.x - x * size, pos.y - y * size, pos.z - z * size);

        children[x + y * 2 + z * 4].set(newPos, value, desiredLevel, level++);
    }
};

struct VoxelOctree {
    uint32_t size;

    void set(uint32_t x, uint32_t y, uint32_t z, uint32_t value) {

    }
};