#include "SceneObject.h"

#include <cstring>
#include <string>
#include <ImGui/imgui.h>
#include <avg/Debug.h>

#include "Rendering/Light.h"

uint32_t SceneObject::ID = 0;

SceneObject::SceneObject() :
type(OBJECT_TYPE_EMPTY), data(nullptr), transform(Transform()) {
    this->id = SceneObject::ID++;

    children = nullptr;
    childrenCount = 0;
    childrenBufferSize = 0;
}

SceneObject::SceneObject(ObjectType type, void* data) :
type(type), data(data), transform(Transform()), children(nullptr) {
    this->id = SceneObject::ID++;
    childrenCount = 0;
    childrenBufferSize = 0;

    switch(type) {
        case OBJECT_TYPE_GRID:
            name = "New Grid";
            break;
        case OBJECT_TYPE_LIGHT:
            name = "New Light";
            break;
        case OBJECT_TYPE_EMPTY:
            name = "New Empty";
            break;
        default:
            name = "New Unknown";
            break;
    }
}

SceneObject::SceneObject(char* name, ObjectType type, void* data) :
type(type), data(data), transform(Transform()), children(nullptr) {
    this->id = SceneObject::ID++;
    
    childrenCount = 0;
    childrenBufferSize = 0;

    this->name = name;
}



void SceneObject::add_child(SceneObject object) {
    // If the resize of the buffer is needed
    if(childrenBufferSize == childrenCount) {
        childrenBufferSize += 8;
        SceneObject* tempBuffer = new SceneObject[childrenBufferSize];

        // Only copy the elements when it is not null (already initialized)
        if(children != nullptr) {
            memcpy(tempBuffer, children, sizeof(SceneObject) * childrenCount);
            delete [] children;
        } children = tempBuffer;
    }

    MESSAGE("Adding child at " << childrenCount << " to " << id);
    children[childrenCount++] = object;
}