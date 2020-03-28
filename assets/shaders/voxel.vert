#version 440 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};

uniform mat4 model;
uniform int index;

out vec3 pos;
out vec3 camPos;
out vec3 normal;
out vec3 shadow;
out vec3 coords;

out float shadowValue;

const float SHADOW_FACTOR = 0.1;

uniform uvec3 size;
uniform sampler3D grid;
uniform sampler1D palette;

void main() {
    coords = vPos * 2.0;
    normal = vNormal;
    camPos = vec3(view[0][3], view[1][3], view[2][3]);

    int offset = index + gl_InstanceID;
    pos = vec3(offset % size.x, offset % (size.x * size.y) / size.z, offset / (size.x * size.y));
    vec3 p = pos + normal;

    shadow = vec3(0.0, 0.0, 0.0);

    if(texelFetch(grid, ivec3(p.x + coords.x - normal.x, p.y, p.z), 0).r != 0)  {
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
        shadowValue = 1.0;
    } else if(texelFetch(grid, ivec3(p.x, p.y + coords.y - normal.y, p.z), 0).r != 0) {
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
        shadowValue = 1.0;
    } else if(texelFetch(grid, ivec3(p.x, p.y, p.z + coords.z - normal.z), 0).r != 0) {
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
        shadowValue = 1.0;
    } else if(texelFetch(grid, ivec3(p.x + coords.x - normal.x, p.y + coords.y - normal.y, p.z + coords.z - normal.z), 0).r != 0) {
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
        shadowValue = 1.0;
    } else {
        shadowValue = 0.0;
    }
    gl_Position = projection * view * model * vec4(vPos + pos + vec3(0.5), 1.0);
}