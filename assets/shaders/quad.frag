#version 440 core

#define MAX_LIGHT_COUNT 32

layout(location = 0) out vec4 fragColor;


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
    fragColor = max(dot(vec3(normalize(lights[0].direction)), normal), 0.1) * color;
    fragColor.w = 1.0;
    //fragColor = vec3(1.0, 1.0, 1.0);
}