#version 440 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};

out vec3 shadow;
out vec3 normal;
out vec3 pos;
out vec3 coords;
out vec3 fragPos;

uniform vec3 position;
uniform vec3 scale;
uniform mat4 model;

uniform sampler3D grid;
uniform sampler1D palette;

out vec4 color;

const float SHADOW_FACTOR = 0.45;

void main() {
    coords = vPos * 2.0;
    pos = position;
    normal = vNormal;

    int value = int(texelFetch(grid, ivec3(pos.x, pos.y, pos.z), 0).r * 255.0);

    color = vec4(texelFetch(palette, value, 0).rgb, 1.0);
    vec3 p = pos + normal;

    shadow = vec3(0.0, 0.0, 0.0);

    if(texelFetch(grid, ivec3(p.x + coords.x - normal.x, p.y, p.z), 0).r != 0)
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
    else if(texelFetch(grid, ivec3(p.x, p.y + coords.y - normal.y, p.z), 0).r != 0)
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
    else if(texelFetch(grid, ivec3(p.x, p.y, p.z + coords.z - normal.z), 0).r != 0)
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
    else if(texelFetch(grid, ivec3(p.x + coords.x - normal.x, p.y + coords.y - normal.y, p.z + coords.z - normal.z), 0).r != 0)
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);

    color.w = 1.0;
    fragPos = vec3(model * vec4(vPos, 1.0));

    gl_Position = projection * view * vec4((vPos + position + vec3(0.5, 0.5, 0.5)) * scale, 1.0);
}