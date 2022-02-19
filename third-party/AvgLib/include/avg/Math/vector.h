#pragma once

#include "float2.h"
#include "float3.h"
#include "float4.h"

namespace avg::vector {
    float length(avg::math::float2 vector);
    float length(avg::math::float3 vector);
    float length(avg::math::float4 vector);

    float length2(avg::math::float2 vector);
    float length2(avg::math::float3 vector);
    float length2(avg::math::float4 vector);

    float dot(avg::math::float2 a, avg::math::float2 b);
    float dot(avg::math::float3 a, avg::math::float3 b);
    float dot(avg::math::float4 a, avg::math::float4 b);

    avg::math::float2 normalize(avg::math::float2 vector);
    avg::math::float3 normalize(avg::math::float3 vector);
    avg::math::float4 normalize(avg::math::float4 vector);

    float distance(avg::math::float2 a, avg::math::float2 b);
    float distance(avg::math::float3 a, avg::math::float3 b);
    float distance(avg::math::float4 a, avg::math::float4 b);
}