#version 440 core

out vec4 fragColor;

in vec2 coords;

uniform sampler2D color;
uniform sampler2D depth;
uniform sampler2D normal;

void main() {
    fragColor = vec4(texture(color, coords).rgb, 1.0);
}