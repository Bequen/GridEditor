#version 440 core


out vec4 fragColor;


in vec3 normal;
in vec4 color;
in vec3 shadow;


void main() {
    vec4 lightDir = normalize(vec4(1.0, 1.0, 10.0, 1.0));
    fragColor = (color) * max(dot(vec3(lightDir), normal), 0.1) - vec4(shadow * 0.1, 0.0);;
    fragColor.w = 1.0;
}