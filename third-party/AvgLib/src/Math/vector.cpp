#include "avg/Math/vector.h"

#include <cmath>

float avg::vector::length(avg::math::float2 vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y);
} float avg::vector::length(avg::math::float3 vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
} float avg::vector::length(avg::math::float4 vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

float avg::vector::length2(avg::math::float2 vector) {
    return vector.x * vector.x + vector.y * vector.y;
} float avg::vector::length2(avg::math::float3 vector) {
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
} float avg::vector::length2(avg::math::float4 vector) {
    return vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;
}

float avg::vector::dot(avg::math::float2 a, avg::math::float2 b) {
    return a.x * b.x + a.y * b.y;
} float avg::vector::dot(avg::math::float3 a, avg::math::float3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
} float avg::vector::dot(avg::math::float4 a, avg::math::float4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

avg::math::float2 normalize(avg::math::float2 vector) {
    float magnitude = avg::vector::length(vector);
    return avg::math::float2(vector.x / magnitude, vector.y / magnitude);
} avg::math::float3 avg::vector::normalize(avg::math::float3 vector) {
    float magnitude = avg::vector::length(vector);
    return avg::math::float3(vector.x / magnitude, vector.y / magnitude, vector.z / magnitude);
} avg::math::float4 avg::vector::normalize(avg::math::float4 vector) {
    float magnitude = avg::vector::length(vector);
    return avg::math::float4(vector.x / magnitude, vector.y / magnitude, vector.z / magnitude);
}

float avg::vector::distance(avg::math::float2 a, avg::math::float2 b) {
    return avg::vector::length(a - b);
} float avg::vector::distance(avg::math::float3 a, avg::math::float3 b) {
    return avg::vector::length(a - b);
} float avg::vector::distance(avg::math::float4 a, avg::math::float4 b) {
    return avg::vector::length(a - b);
}