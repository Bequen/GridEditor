#version 440 core

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

void main() {
    normal = vNormal;
    if(scale.x > 0 && scale.y > 0)
        color = vec4(1.0, 0.0, 0.0, 1.0);
    else if(scale.x > 0 && scale.y <= 0)
        color = vec4(0.0, 1.0, 0.0, 1.0);
    else if(scale.x <= 0 && scale.y > 0)
        color = vec4(0.0, 0.0, 1.0, 1.0);

    vec3 quadScale = vec3(scale.x - position.x, scale.y - position.y, scale.z - position.z);
    gl_Position = projection * view * vec4(vPos * quadScale + position + vec3(max(normal.x, 0.0), max(normal.y, 0.0), max(normal.z, 0.0)), 1.0);
}