#version 440 core

const float SHADOW_FACTOR = 0.45;

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};

uniform vec3 position;
uniform vec3 scale;

out vec3 normal;
out vec4 color;
out vec3 shadow;


uniform sampler3D grid;
uniform sampler1D palette;


void main() {
    vec3 coords = vPos * 2.0;
    normal = vNormal;
    vec3 pos = position;

    int value = int(texelFetch(grid, ivec3(pos.x, pos.y, pos.z), 0).r * 255.0);

    color = vec4(texelFetch(palette, value, 0).rgb, 1.0);
    vec3 p = pos + normal;

    shadow = vec3(0.0, 0.0, 0.0);

    /* if(texelFetch(grid, ivec3(p.x + coords.x - normal.x, p.y, p.z), 0).r != 0)
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
    else if(texelFetch(grid, ivec3(p.x, p.y + coords.y - normal.y, p.z), 0).r != 0)
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
    else if(texelFetch(grid, ivec3(p.x, p.y, p.z + coords.z - normal.z), 0).r != 0)
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
    else if(texelFetch(grid, ivec3(p.x + coords.x - normal.x, p.y + coords.y - normal.y, p.z + coords.z - normal.z), 0).r != 0)
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR); */

    vec3 quadScale = vec3(scale.x - position.x, scale.y - position.y, scale.z - position.z);
    gl_Position = projection * view * vec4(vPos * quadScale + position + vec3(max(normal.x, 0.0), max(normal.y, 0.0), max(normal.z, 0.0)), 1.0);
}