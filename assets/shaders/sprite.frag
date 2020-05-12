#version 440 core

in vec2 coords;
in vec2 pos;

out vec4 fragColor;

uniform sampler2D spriteTexture;

struct Material {
    vec4 albedo;
    vec4 brdf;
};

layout(binding = 3, std140) uniform Materials {
    Material materials[256];
};

void main() {
    int value = int(texelFetch(spriteTexture, ivec2(pos.x, pos.y), 0).r * 255.0);
    vec3 color = materials[value].albedo.xyz;

    fragColor = vec4(color, 1.0);
}