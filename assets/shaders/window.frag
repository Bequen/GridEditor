#version 440 core

out vec4 fragColor;

uniform vec4 color;

void main() {
    fragColor = color;
}