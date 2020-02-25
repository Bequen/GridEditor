#version 440 core

layout(location = 0) out vec4 color;

in vec3 normal;
in vec3 pos;
in vec3 coords;

vec2 brickTile(vec2 _st, float _zoom){
    _st *= _zoom;

    return fract(_st);
}

float box(vec2 _st, vec2 _size){
    _size = _size * 0.025;
    vec2 uv = smoothstep(_size, _size + vec2(1e-1), _st);
    uv *= smoothstep(_size, _size+vec2(1e-1), vec2(1.0)-_st);
    return uv.x * uv.y;
}

void main() {
    vec2 c = vec2(0.0);
    if(normal.x != 0.0)
        c = vec2(coords.y, coords.z);
    else if(normal.y != 0.0)
        c = vec2(coords.x, coords.z);
    else if(normal.z != 0.0)
        c = vec2(coords.x, coords.y);
    vec2 st = c / 1.0;
    vec3 col = vec3(0.0);

    st = brickTile(st, 128.0);

    //color = vec3(1.0 - box(st,vec2(0.5)));
    color = vec4(0.0, 0.0, 0.0, vec3(1.0 - box(st,vec2(0.5))));
}
