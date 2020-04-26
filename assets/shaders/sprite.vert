#version 440 core

layout(location = 0) in vec2 vPos;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;

out vec2 coords;

void main() {
    coords = vPos.xy;
    gl_Position = projection * view * model * vec4(vPos, 0.0, 1.0);
}