#pragma once

#include "float2.h"
#include "float3.h"

namespace avg::math {
    struct float4 {
        float x, y, z, w;

        float4() :
        x(0.0f), y(0.0f), z(0.0f), w(1.0f) {

        } 

        float4(float value) :
        x(value), y(value), z(value), w(1.0f) {

        }

        float4(float x, float y, float z) :
        x(x), y(y), z(z), w(1.0f) {

        }

        float4(float x, float y, float z, float w) :
        x(x), y(y), z(z), w(w) {

        }

        friend float4 operator+(float4 lhs, const float4& rhs) {
            return float4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
        } friend float4 operator+(float4 lhs, const float& rhs) {
            return float4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
        } friend float4 operator-(float4 lhs, const float4& rhs) {
            return float4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
        } friend float4 operator-(float4 lhs, const float& rhs) {
            return float4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
        }

        friend float4 operator*(float4 lhs, const float4& rhs) {
            return float4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
        } friend float4 operator*(float4 lhs, const float& rhs) {
            return float4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
        }

        float4& operator+=(const float4& rhs) {
            this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; this->w += rhs.w;
            return *this;
        } float4& operator-=(const float4& rhs) {
            this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; this->w -= rhs.w;
            return *this;
        }

        float4& operator+=(const float& rhs) {
            this->x += rhs; this->y += rhs; this->z += rhs; this->w += rhs;
            return *this;
        } float4& operator-=(const float& rhs) {
            this->x -= rhs; this->y -= rhs; this->z -= rhs; this->w -= rhs;
            return *this;
        }
    };
}