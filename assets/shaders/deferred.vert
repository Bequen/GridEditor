#version 440 core

layout(location = 0) in vec2 vPos;

out vec2 coords;

void main() {
    coords = (vPos + vec2(1.0, 1.0)) / 2.0;
    gl_Position = vec4(vPos.x, vPos.y, 0.0, 1.0);
}