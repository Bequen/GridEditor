#version 440 core

layout(location = 0) out vec4 fragColor;

in vec3 normal;
in vec3 pos;
in vec3 coords;

uniform vec4 skyColor;

void main() {
    fragColor = skyColor;
    fragColor.w = 1.0;
}