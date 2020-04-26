#version 440 core

in vec2 coords;

out vec4 fragColor;

uniform sampler2D spriteTexture;

void main() {
    fragColor = texture(spriteTexture, coords).rgba;
}