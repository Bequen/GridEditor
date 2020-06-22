#pragma once

#include <cstdint>
#include <vector>

#include "System/Transform.h"

/**
 * @brief  Defines what EditorSceneObject or SceneObject is
 * @note   
 * @retval None
 */
typedef uint32_t ObjectType;

#define OBJECT_TYPE_EMPTY       0x0000
#define OBJECT_TYPE_GRID        0x0001
#define OBJECT_TYPE_LIGHT       0x0002
#define OBJECT_TYPE_PARTICLES   0x0003
#define OBJECT_TYPE_SPRITE      0x0004


/**
 * @brief  Basic unit in the scene
 * @note   Scene object is flexible way to describe any form of unit        
 * * inside of the scene. SceneObject can be light, grid, or 
 * * sprite.
 * @retval None
 */
struct SceneObject {
    static uint32_t ID;
    uint32_t id;

    char* name;
    Transform transform;

    ObjectType type;
    void* data;

    SceneObject* children;
    uint32_t childrenCount;
    uint32_t childrenBufferSize;

    SceneObject();
    SceneObject(ObjectType type, void* data);
    SceneObject(char* name, ObjectType type, void* data);

    void add_child(SceneObject object);

    void assign(ObjectType type, void* data);

    const char* get_name();
    void set_name(char* name);
};