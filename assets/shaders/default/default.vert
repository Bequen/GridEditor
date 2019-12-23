#version 440 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};

out vec3 normal;

uniform vec3 position;
uniform mat4 model;

void main() {
    normal = vNormal;
    gl_Position = projection * view * model * vec4(vPos + position + vec3(0.5, 0.5, 0.5), 1.0);
}