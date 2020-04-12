#include "InputManager.h"

#include <avg/Debug.h>

InputManager Input;

void InputManager::init(uint32_t count) {
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

    glfwGetFramebufferSize(window.window, &windowWidth, &windowHeight);
}

void InputManager::add_key(InputKey key) {
    if(keyCount >= bufferSize) {
        realloc(keys, (bufferSize += 50));
    } 
    keys[keyCount++] = key;
} void InputManager::add_mouse_key(InputKey key) {
    if(mouseKeyCount >= mouseKeyBufferSize) {
        realloc(mouseKeys, (mouseKeyBufferSize += 50));
    } 
    mouseKeys[mouseKeyCount++] = key;
}



void InputManager::update()  {
    deltaTime = glfwGetTime() - lastTime;
    lastTime = glfwGetTime();

    glfwGetFramebufferSize(window.window, &windowWidth, &windowHeight);

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


uint32_t InputManager::get(uint32_t key)  {
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


void InputManager::get_mapped_cursor(WindowTileInfo tile, double* x, double* y) {
    assert_msg(x != nullptr && y != nullptr, "One of the arguments are null");

    double cursorX = (mouseLastX + 1.0f) / 2.0f;
    double cursorY = (mouseLastY + 1.0f) / 2.0f;
    glfwGetCursorPos(window.window, &cursorX, &cursorY);

    double resultX = (cursorX - tile.x * windowWidth) / tile.width / windowWidth;
    double resultY = 1.0 - (cursorY - tile.y * windowHeight) / tile.height / windowHeight;

    *x = resultX * 2.0f - 1.0f;
    *y = resultY * 2.0f - 1.0f;
}

void InputManager::set_cursor(uint32_t cursor) {
    window.set_cursor(cursor);
}