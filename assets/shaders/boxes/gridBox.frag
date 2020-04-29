#version 440 core

layout(location = 0) out vec4 color;

in vec3 normal;
in vec3 pos;
in vec3 coords;

uniform vec3 scale;

void main() {
    // From http://madebyevan.com blog
    // Compute anti-aliased world-space grid lines
    vec3 grid = abs(fract(coords * 32.0 - 0.2) - 0.8) / fwidth(coords * 32.0);
    float line = min(min(grid.x, grid.y), grid.z);

    vec3 border = abs((fract(coords - 0.5) - 0.5)) / fwidth(coords);
    float borderLine = min(min(border.x, border.y), border.z);
    // Just visualize the grid lines directly
    color = vec4(vec3(0.5) - (1.0 - min(borderLine, 1.0)), 1.0 - min(line, 1.0));
}
