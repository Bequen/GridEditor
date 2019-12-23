#version 440 core

out vec4 color;

in vec3 normal;

void main() {
    vec4 lightDir = vec4(1.0, -1.0, 1.0, 1.0);

    
    color = vec4 (1.0, 0.0, 0.0, 1.0) * dot(vec3(lightDir), normal);
}