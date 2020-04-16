#version 440 core

#define PI 3.14159265359
#define MAX_LIGHT_COUNT 32

#define LIGHT_TYPE_DIRECTIONAL      0.0
#define LIGHT_TYPE_POINT            1.0
#define LIGHT_TYPE_SPOT             2.0

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

in vec3 fragPos;
in vec3 pos;
in vec3 camPos;
in vec3 normal;
in vec3 coords;
in vec3 shadow;
in float shadowValue;

flat in vec3 vertPos;
in vec3 fPos;

float roughness = 1.0;
float metalness = 0.0;

vec3 fresnel_schlick(float cosTheta, vec3 f0) {
    return f0 + (1.0 - f0) * pow(1.0 - cosTheta, 5.0);
}

float normal_distribution_ggx(vec3 normal, vec3 halfway) {
    float a2 = roughness * roughness;
    float NdotH = max(dot(normal, halfway), 0.0);

    float denom = (NdotH * NdotH * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / denom;
}

float geometry_schlick_ggx(float NdotV) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;

    return NdotV / denom;
}

float geometry_smith(vec3 normal, vec3 viewDir, vec3 lightDir) {
    float NdotV = max(dot(normal, viewDir), 0.0);
    float NdotL = max(dot(normal, lightDir), 0.0);

    float ggx1 = geometry_schlick_ggx(NdotV);
    float ggx2 = geometry_schlick_ggx(NdotL);

    return ggx1 * ggx2;
}

void main() {
    int value = int(texelFetch(grid, ivec3(pos.x, pos.y, pos.z), 0).r * 255.0);
    vec3 color = vec3(texelFetch(palette, value, 0).rgb);

    vec3 Lo = vec3(0.0);

    vec3 viewDir = normalize(camPos - fragPos);
    vec3 f0 = vec3(0.04);
    f0 = mix(f0, color, metalness);
    for(int i = 0; i < 1; i++) {
        vec3 lightDir = vec3(0.0);

        float d = length(lights[i].position.xyz - fragPos);
        float attenuation = 0.0;
        if(lights[i].position.w == LIGHT_TYPE_DIRECTIONAL) {
            lightDir = normalize(lights[i].direction.xyz);
            attenuation = lights[i].albedo.w;
        } else if(lights[i].position.w == LIGHT_TYPE_POINT) {
            lightDir = normalize(lights[i].position.xyz - fragPos);
            attenuation = 1.0 / (d * d) * lights[i].albedo.w;
        } else {
            lightDir = normalize(lights[i].direction.xyz);
            attenuation = lights[i].albedo.w;
        }

        vec3 halfway = normalize(lightDir + viewDir);

        vec3 radiance = lights[i].albedo.xyz * attenuation;

        vec3 fresnel = fresnel_schlick(max(dot(halfway, viewDir), 0.0), f0);
        float distrubution = normal_distribution_ggx(normal, halfway);
        float smith = geometry_smith(normal, viewDir, lightDir);

        vec3 cookTorrance = distrubution * smith * fresnel;
        cookTorrance = cookTorrance / max(4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, lightDir), 0.0), 0.001);

        vec3 kS = fresnel;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metalness;

        float NdotL = max(dot(normal, lightDir), 0.0);
        Lo += (kD * color / PI + cookTorrance) * radiance * NdotL * (1.0 - shadowValue);
    }

    vec3 ambient = vec3(0.529411765, 0.807843137, 0.921568627) * color;
    vec3 final = ambient + Lo;

    final = final / (final + vec3(1.0));
    final = pow(final, vec3(1.0 / 2.2));

    fragColor = vec4(final, 1.0);
}