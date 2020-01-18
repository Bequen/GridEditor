#version 440 core

const float SHADOW_FACTOR = 1.0;

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
    normal = vNormal;
    vec3 offset = vec3(max(normal.x, 0.0), max(normal.y, 0.0), max(normal.z, 0.0));
    vec3 coords = (vPos - vec3(0.5, 0.5, 0.0)) * 2.0;
    vec3 quadScale = vec3(scale.x - position.x, scale.y - position.y, scale.z - position.z);
    vec3 pos = position + ((vPos - vec3(0.5, 0.5, 0.0)) * 2.0);

    int value = int(texelFetch(grid, ivec3(position.x, position.y, position.z)/* ivec3(0, 0, 0) */, 0).r * 255.0);

    color = vec4(texelFetch(palette, value, 0).rgb, 1.0);
    //color = vec4(value / 255, value / 255, value / 255, 1.0);
    vec3 p = pos + normal;
    
    gl_Position = projection * view * vec4(vPos * quadScale + position + offset, 1.0);
}