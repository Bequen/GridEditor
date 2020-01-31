#version 440 core

layout(location = 0) in vec3 vPos;

uniform vec4 transform;

void main() {
    vec2 pos = ((vec2(vPos.x, vPos.y) + 1.0) / 2.0) * (transform.zw) + transform.xy;
    gl_Position = vec4(pos.x * 2.0 - 1.0, pos.y * 2.0 - 1.0, 0.0, 1.0);
}