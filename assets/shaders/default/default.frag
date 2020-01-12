#version 440 core

out vec4 fragColor;

in vec4 color;
in vec3 normal;
in vec3 pos;
in vec3 coords;
in vec3 shadow;
in vec3 fragPos;

uniform sampler3D grid;
uniform sampler1D palette;

void main() {
    vec4 lightDir = normalize(vec4(1.0, 1.0, 10.0, 1.0));
    //int value = int(texelFetch(grid, ivec3(pos.x, pos.y, pos.z), 0).r * 255.0);

    //fragColor = vec4(texelFetch(palette, value, 0).rgb * dot(vec3(lightDir), normal), 1.0);
    fragColor = (color) * max(dot(vec3(lightDir), normal), 0.1) - vec4(shadow * 0.1, 0.0);
    fragColor.w = 1.0;
}
