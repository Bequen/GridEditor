#version 440 core

out vec4 color;

in vec3 normal;
in vec3 pos;
in vec3 coords;

uniform sampler3D grid;

void main() {
    vec4 lightDir = vec4(1.0, -1.0, 1.0, 1.0);
    int value = int(texelFetch(grid, ivec3(pos.x, pos.y, pos.z), 0).r * 255);
    if(value == 1) {
        color = vec4 (0.0, 1.0, 0.0, 1.0) * dot(vec3(lightDir), normal);
    } else if(value == 2) {
        color = vec4 (1.0, 0.0, 0.0, 1.0) * dot(vec3(lightDir), normal);
    }
}