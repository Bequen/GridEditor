#version 440 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

//layout(location = 2) in int index;

layout(binding = 0, std140) uniform Camera {
    mat4 projection;
    mat4 view;
};


uniform mat4 model;
uniform int index;
uniform int subgrid;
uniform int subgridLength;
uniform vec3 cameraPos;

out vec3 pos;
out vec3 fragPos;
out vec3 camPos;
out vec3 normal;
out vec3 shadow;
out vec3 coords;

flat out vec3 vertPos;
out vec3 fPos;

out float shadowValue;

const float SHADOW_FACTOR = 0.1;

uniform uvec3 size;
uniform sampler3D grid;
uniform sampler1D palette;

void main() {
    vertPos = vPos;
    fPos = vPos;
    coords = vPos * 2.0;
    normal = vNormal;
    camPos = (vec4(view[3][0], view[3][1], view[3][2], 1.0) * transpose(view)).xyz;
    camPos = cameraPos;

    vec3 subgridOffset = vec3(subgrid % subgridLength, (subgrid % (subgridLength * subgridLength)) / subgridLength, subgrid / (subgridLength * subgridLength));

    int offset = index + gl_InstanceID;
    pos = vec3(offset % size.x, offset % (size.x * size.y) / size.z, offset / (size.x * size.y));
    vec3 p = pos + normal;

    shadow = vec3(0.0, 0.0, 0.0);

    // Ambient occlusion
    shadowValue = 0.0;
    if(texelFetch(grid, ivec3(p.x + coords.x - normal.x, p.y, p.z), 0).r != 0)  {
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
        shadowValue += 0.3;
    } if(texelFetch(grid, ivec3(p.x, p.y + coords.y - normal.y, p.z), 0).r != 0) {
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
        shadowValue += 0.3;
    } if(texelFetch(grid, ivec3(p.x, p.y, p.z + coords.z - normal.z), 0).r != 0) {
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
        shadowValue += 0.3;
    } if(texelFetch(grid, ivec3(p.x + coords.x - normal.x, p.y + coords.y - normal.y, p.z + coords.z - normal.z), 0).r != 0) {
        shadow += vec3(SHADOW_FACTOR, SHADOW_FACTOR, SHADOW_FACTOR);
        shadowValue += 0.3;
    } 

    fragPos = (vec4(vPos + pos + subgridOffset * 8 + vec3(0.5), 1.0)).xyz;
    gl_Position = projection * view * vec4(vPos + pos + subgridOffset * 8 + vec3(0.5), 1.0);
}
