#version 440 core

out vec4 fragColor;

in vec2 coords;

uniform sampler2D color;
uniform sampler2D depth;

void main() {
    fragColor = vec4(texture(color, coords).rgb, 1.0);
    /* fragColor = vec4(1.0, 0.0, 1.0, 1.0); */
}