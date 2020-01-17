#version 440 core

#define MAX_LIGHT_COUNT 32

out vec4 fragColor;


in vec3 normal;
in vec4 color;
in vec3 shadow;

struct Light {
    vec4 position;
    vec4 direction;

    vec4 ambient;
};

layout(binding = 2, std140) uniform Lights {
    Light lights[MAX_LIGHT_COUNT];
    vec4 skyColor;
};


void main() {
    vec4 lightDir = normalize(vec4(1.0, 1.0, 10.0, 1.0));
    fragColor = mix(vec4(0.0, 0.0, 0.0, 1.0), color, max(dot(vec3(lights[0].direction), normal), 0.5));
    fragColor.w = 1.0;
}