#version 440 core

layout(location = 0) in vec3 vPos;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform int index;

out vec2 coords;
out vec2 pos;

void main() {
    vec3 offset = vec3((index + gl_InstanceID) % 32, 0.0, (index + gl_InstanceID) / 32);

    pos = offset.xz;
    coords = vPos.xz / 32.0;
    gl_Position = projection * view * vec4(vPos + offset + 0.5, 1.0);
}