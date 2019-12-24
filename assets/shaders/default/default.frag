#version 440 core

out vec4 color;

in vec3 normal;
in vec3 pos;
in vec3 coords;
in vec3 shadow;

uniform sampler3D grid;
uniform sampler1D palette;

void main() {
    vec4 lightDir = normalize(vec4(1.0, 10.0, 1.0, 1.0));
    int value = int(texelFetch(grid, ivec3(pos.x, pos.y, pos.z), 0).r * 255.0);

    color = vec4(texelFetch(palette, value, 0).rgb * dot(vec3(lightDir), normal), 1.0);
}
