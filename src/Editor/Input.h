#pragma once

#include <cstdint>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define KEY_STATE_NONE 0x0000
#define KEY_STATE_PRESS 0x0001
#define KEY_STATE_HELD 0x0002

#include "System/Window.h"

struct InputKey {
    uint32_t key;
    uint32_t state;
};

// TODO Probably some sort of hashing would be cool
struct Input {
    InputKey* keys;
    uint32_t keyCount;
    uint32_t bufferSize;

    InputKey* mouseKeys;
    uint32_t mouseKeyCount;
    uint32_t mouseKeyBufferSize;

    double mouseDeltaX;
    double mouseDeltaY;

    double mouseLastX;
    double mouseLastY;

    double lastTime;
    double deltaTime;

    void init(uint32_t count) {
        lastTime = 0.0;
        deltaTime = 0.0;
        keys = new InputKey[count];
        keyCount = 0;
        bufferSize = count;

        mouseKeys = new InputKey[count];
        mouseKeyCount = 0;
        mouseKeyBufferSize = count;

        mouseDeltaX = mouseDeltaY = 0.0;
        mouseLastX = mouseLastY = 0.0;
    }

    void add_key(InputKey key) {
        if(keyCount >= bufferSize) {
            realloc(keys, (bufferSize += 50));
        } 
        keys[keyCount++] = key;
    } void add_mouse_key(InputKey key) {
        if(mouseKeyCount >= mouseKeyBufferSize) {
            realloc(mouseKeys, (mouseKeyBufferSize += 50));
        } 
        mouseKeys[mouseKeyCount++] = key;
    }

    void update(Window window) {

        deltaTime = glfwGetTime() - lastTime;
        lastTime = glfwGetTime();

        glm::vec3 cursor = window.get_normalized_cursor_pos();
        mouseDeltaX = mouseLastX - cursor.x;
        mouseDeltaY = mouseLastY - cursor.y;

        mouseLastX = cursor.x;
        mouseLastY = cursor.y;

        for(uint32_t i = 0; i < keyCount; i++) {
            if(window.is_key_down(keys[i].key) == GLFW_PRESS) {
                if(keys[i].state == KEY_STATE_PRESS)
                    keys[i].state = KEY_STATE_HELD;
                else if(keys[i].state == KEY_STATE_NONE)
                    keys[i].state = KEY_STATE_PRESS;  
            } else {
                keys[i].state = KEY_STATE_NONE;
            }
        } for(uint32_t i = 0; i < mouseKeyCount; i++) {
            if(window.is_mouse_button_down(mouseKeys[i].key) == GLFW_PRESS) {
                if(mouseKeys[i].state == KEY_STATE_PRESS)
                    mouseKeys[i].state = KEY_STATE_HELD;
                else if(mouseKeys[i].state == KEY_STATE_NONE)
                    mouseKeys[i].state = KEY_STATE_PRESS;  
            } else {
                mouseKeys[i].state = KEY_STATE_NONE;
            }
        }
    }

    uint32_t get(uint32_t key) {
        for(uint32_t i = 0; i < keyCount; i++)
            if(keys[i].key == key)
                return keys[i].state;
        for(uint32_t i = 0; i < mouseKeyCount; i++) {
            if(mouseKeys[i].key == key) {
                return mouseKeys[i].state;
            }
        }

        return 0;
    }
};