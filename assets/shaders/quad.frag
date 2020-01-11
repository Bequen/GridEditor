#version 440 core

in vec3 normal;
out vec4 fragColor;
in vec4 color;

void main() {
    vec4 lightDir = normalize(vec4(1.0, 10.0, 1.0, 1.0));
    fragColor = (color) * max(dot(vec3(lightDir), normal), 0.1);
    fragColor.w = 1.0;
    fragColor = color;
}