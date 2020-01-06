#version 440 core

out vec4 color;

in vec3 normal;
in vec3 pos;
in vec3 coords;

void main() {
    vec4 lightDir = normalize(vec4(1.0, 10.0, 1.0, 1.0));
    color = vec4(vec3(0.2, 0.2, 0.2) * dot(vec3(lightDir), normal), 0.4);
}
