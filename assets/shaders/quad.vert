#version 440 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};

uniform vec3 position;
uniform vec2 scale;

out vec3 normal;
out vec4 color;

void main() {
    normal = vNormal;
    color = vec4(1.0, 0.0, 0.0, 1.0);


    gl_Position = projection * view * vec4(vPos * vec3(scale.x - position.x, 1.0, scale.y) + position, 1.0);
}