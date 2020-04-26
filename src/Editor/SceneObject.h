#pragma once

#include <cstdint>
#include <vector>

#include "System/Transform.h"

#define SCENE_GRID          0x0000
#define SCENE_LIGHT         0x0001
#define SCENE_SPRITE        0x0002
#define SCENE_PARTICLES     0x0003

typedef uint32_t ObjectType;

#define OBJECT_TYPE_EMPTY       0x0000
#define OBJECT_TYPE_GRID        0x0001
#define OBJECT_TYPE_LIGHT       0x0002
#define OBJECT_TYPE_PARTICLES   0x0003
#define OBJECT_TYPE_SPRITE      0x0004


struct SceneObject {
    static uint32_t ID;
    uint32_t id;

    ObjectType type;
    void* data;

    char* name;

    Transform transform;

    SceneObject* children;
    uint32_t childrenCount;
    uint32_t childrenBufferSize;

    SceneObject();
    SceneObject(ObjectType type, void* data);
    SceneObject(char* name, ObjectType type, void* data);

    void add_child(SceneObject object);
    void assign(ObjectType type, void* data);
};