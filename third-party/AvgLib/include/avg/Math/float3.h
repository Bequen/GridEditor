#pragma once

namespace avg::math {
    struct float3 {
        float x, y, z;

        float3() :
        x(0.0f), y(0.0f), z(0.0f) {

        } 

        float3(float value) :
        x(value), y(value), z(value) {

        }

        float3(float x, float y, float z) :
        x(x), y(y), z(z) {

        }

        friend float3 operator+(float3 lhs, const float3& rhs) {
            return float3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
        } friend float3 operator+(float3 lhs, const float& rhs) {
            return float3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
        } friend float3 operator-(float3 lhs, const float3& rhs) {
            return float3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
        } friend float3 operator-(float3 lhs, const float& rhs) {
            return float3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
        }

        friend float3 operator*(float3 lhs, const float3& rhs) {
            return float3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
        } friend float3 operator*(float3 lhs, const float& rhs) {
            return float3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
        }

        float3& operator+=(const float3& rhs) {
            this->x += rhs.x; this->y += rhs.y; this->z += rhs.z;
            return *this;
        } float3& operator-=(const float3& rhs) {
            this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z;
            return *this;
        }

        float3& operator+=(const float& rhs) {
            this->x += rhs; this->y += rhs; this->z += rhs;
            return *this;
        } float3& operator-=(const float& rhs) {
            this->x -= rhs; this->y -= rhs; this->z -= rhs;
            return *this;
        }
    };
}