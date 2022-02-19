#include "SceneObject.h"

#include <cstring>
#include <string>
#include <imgui/imgui.h>
#include <avg/Debug.h>

#include "Rendering/Light.h"

uint32_t SceneObject::ID = 0;

SceneObject::SceneObject() :
type(OBJECT_TYPE_EMPTY), data(nullptr), transform(Transform()) {
    this->id = SceneObject::ID++;

    name = new char[256];

    children = nullptr;
    childrenCount = 0;
    childrenBufferSize = 0;
}

SceneObject::SceneObject(ObjectType type, void* data) :
type(type), data(data), transform(Transform()), children(nullptr) {
    this->id = SceneObject::ID++;
    childrenCount = 0;
    childrenBufferSize = 0;
    name = new char[256];

    switch(type) {
        case OBJECT_TYPE_GRID:
            set_name("New Grid");
            break;
        case OBJECT_TYPE_LIGHT:
            set_name("New Light");
            break;
        case OBJECT_TYPE_EMPTY:
            set_name("New Empty");
            break;
        case OBJECT_TYPE_SPRITE:
            set_name("New Sprite");
            break;
        default:
            set_name("New Unknown");
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

void SceneObject::assign(ObjectType type, void* data) {
    this->type = type;
    this->data = data;
}

void SceneObject::set_name(char* name) {
    uint32_t length = strlen(name);
    if(length > 256)
        length = 256;
    memcpy(this->name, name, length);
    this->name[length] = '\0';
}

const char* SceneObject::get_name() {
    return name;
}

SceneObject* SceneObject::remove(SceneObject* obj) {
    SceneObject* result = nullptr;
    
    for(uint32_t i = 0; i < childrenCount; i++) {
        if(result != nullptr)
            break;
        else if(children[i].id == obj->id) {
            // TODO Figure out if order matters, otherwise just move the very last element here
            memmove(children + i, children + i + 1, (childrenCount - i) * sizeof(SceneObject));
            childrenCount--;
        } else {
            result = children[i].remove(obj);
        }
    }

    return result;
}
