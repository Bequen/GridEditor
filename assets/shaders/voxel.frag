#version 440 core

#define PI 3.14159265359
#define MAX_LIGHT_COUNT 32

layout(location = 0) out vec4 fragColor;


struct Light {
    vec4 position;
    vec4 direction;

    vec4 albedo;
};

layout(binding = 2, std140) uniform Lights {
    Light lights[MAX_LIGHT_COUNT];
}; 
uniform int lightCount;


uniform int index;

uniform sampler3D grid;
uniform sampler1D palette;

in vec3 pos;
in vec3 camPos;
in vec3 normal;
in vec3 coords;
in vec3 shadow;
in float shadowValue;


float roughness = 0.8;


float trowbridge_normal_distribution(vec3 halfway) {
    float a2 = roughness * roughness;
    float divider = PI * pow(pow(dot(normal, halfway), 2) * (a2 - 1) + 1, 2);

    return a2 / divider;
}

float schlick_geometry(float d, float k) {
    return d / (d * (1.0 - k) + k);
}

vec3 fresnel_schlick(float theta, vec3 f0) {
    return f0 + (1.0 - f0) * pow(1.0 - theta, 5.0); 
}

float smith_ggx(vec3 normal, vec3 lightDir, vec3 viewDir, float k) {
    float ggx1 = schlick_geometry(dot(lightDir, normal), k);
    float ggx2 = schlick_geometry(dot(viewDir, normal), k);
    return ggx1 * ggx2;
}

void main() {
    int value = int(texelFetch(grid, ivec3(pos.x, pos.y, pos.z), 0).r * 255.0);
    vec3 color = vec3(texelFetch(palette, value, 0).rgb);

    vec3 viewDir = normalize(camPos - pos);
    vec3 lightDir = normalize(lights[0].direction).xyz;
    vec3 halfway = normalize(viewDir + lightDir);

    float metallic = 0.1;
    vec3 f0 = vec3(0.04); 
    f0 = mix(f0, color, metallic);

    float k = (roughness + 1) * (roughness + 1) / 8;
    float ndf = trowbridge_normal_distribution(halfway);
    float smith = smith_ggx(normal, lightDir, viewDir, k);
    vec3 fresnel = fresnel_schlick(clamp(dot(halfway, viewDir), 0.0, 1.0), f0);

    vec3 nominator = ndf * smith * fresnel;
    float denominator = 4 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0);
    vec3 specular = nominator / max(denominator, 0.001);

    vec3 kS = fresnel;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float d = max(dot(normal, lightDir), 0.0);
    vec3 Lo = (kD * color / PI + specular) * 1.0 * d;

    vec3 ambient = vec3(0.03) * color * (1.0 - shadow * 0.1);
    vec3 result = ambient + Lo;

    result = pow(result, vec3(1.0/2.2)); 

    fragColor = vec4(color * max(dot(lightDir, normal), 0.1) - shadowValue * 0.05, 1.0);
}