#version 440 core

in vec3 normal;
in vec3 pos;
in vec3 coords;

uniform vec4 skyColor;

out vec4 fragColor;

void main() {
    fragColor = skyColor;
    fragColor.w = 1.0;
}