#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform {
    glm::mat4 transform;

    glm::vec3 forward() {
        return -transform[2];
    } glm::vec3 back() {
        return transform[2];
    }

    glm::vec3 left() {
        return -transform[0];
    } glm::vec3 right() {
        return transform[0];
    }

    glm::vec3 up() {
        return transform[1];
    } glm::vec3 down() {
        return -transform[1];
    }
};