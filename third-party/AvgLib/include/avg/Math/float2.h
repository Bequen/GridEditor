#pragma once

namespace avg::math {
    struct float2 {
        float x, y;

        float2() :
        x(0.0f), y(0.0f) {

        } 

        float2(float value) :
        x(value), y(value) {

        }

        float2(float x, float y) :
        x(x), y(y) {

        }
        
        friend float2 operator+(float2 lhs, const float2& rhs) {
            return float2(lhs.x + rhs.x, lhs.y + rhs.y);
        } friend float2 operator+(float2 lhs, const float& rhs) {
            return float2(lhs.x + rhs, lhs.y + rhs);
        } friend float2 operator-(float2 lhs, const float2& rhs) {
            return float2(lhs.x - rhs.x, lhs.y - rhs.y);
        } friend float2 operator-(float2 lhs, const float& rhs) {
            return float2(lhs.x - rhs, lhs.y - rhs);
        }

        friend float2 operator*(float2 lhs, const float2& rhs) {
            return float2(lhs.x * rhs.x, lhs.y * rhs.y);
        } friend float2 operator*(float2 lhs, const float& rhs) {
            return float2(lhs.x * rhs, lhs.y * rhs);
        }

        float2& operator+=(const float2& rhs) {
            this->x += rhs.x; this->y += rhs.y;
            return *this;
        } float2& operator-=(const float2& rhs) {
            this->x -= rhs.x; this->y -= rhs.y;
            return *this;
        }

        float2& operator+=(const float& rhs) {
            this->x += rhs; this->y += rhs;
            return *this;
        } float2& operator-=(const float& rhs) {
            this->x -= rhs; this->y -= rhs;
            return *this;
        }
    };
}