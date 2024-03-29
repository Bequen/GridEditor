#version 440 core


// Shader used to render bounding boxes


layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};

out vec3 normal;
out vec3 pos;
out vec3 coords;
out vec3 fragPos;

uniform vec3 position;
uniform vec3 scale;
uniform mat4 model;

void main() {
    coords = vPos + 0.5;
    pos = position;
    normal = vNormal;

    gl_Position = projection * view * vec4((vPos + position + vec3(0.5, 0.5, 0.5)) * scale, 1.0);
}