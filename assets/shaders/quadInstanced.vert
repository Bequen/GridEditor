#version 440 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;

layout(location = 5) in vec3 qPos;
layout(location = 6) in vec3 qScale;

void main() {
    gl_Position = vec4(vPos * qScale + qPos, 1.0);
}