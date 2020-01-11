#version 440 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};

out vec3 normal;
out vec3 pos;
out vec3 coords;

uniform vec3 position;
uniform vec3 scale;
uniform mat4 model;

void main() {
    coords = vPos + 0.5;
    pos = position;
    normal = vNormal;

    gl_Position = projection * vec4(vPos, 1.0);
}