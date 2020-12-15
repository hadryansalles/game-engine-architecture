#pragma once

#include <iostream>
#include <cmath>

class Vec3 {
public:
    
    union {
        struct {
            float x;
            float y;
            float z;
        };

        struct {
            float r;
            float g;
            float b;
        };

        float data[3];
    };

    Vec3()
        : x(.0f)
        , y(.0f)
        , z(.0f)
    {}

    Vec3(float xyz)
        : x(xyz)
        , y(xyz)
        , z(xyz)
    {}

    Vec3(float x, float y, float z)
        : x(x)
        , y(y)
        , z(z)
    {}

    Vec3& operator+=(const Vec3& rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return (*this);
    }

    Vec3 operator+(const Vec3& rhs) {
        return Vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vec3& operator-=(const Vec3& rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return (*this);
    }

    Vec3 operator-(const Vec3& rhs) {
        return Vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vec3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return (*this);
    }

    Vec3 operator*(float scalar) {
        return Vec3(x*scalar, y*scalar, z*scalar);
    }

    Vec3& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return (*this);
    }

    Vec3 operator/(float scalar) {
        return Vec3(x/scalar, y/scalar, z/scalar);
    }

    float Mag() {
        return std::sqrt(Mag2());
    }
    
    float Mag2() {
        return x * x + y * y + z * z;
    }

    float Dot(const Vec3& rhs) {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    Vec3 Cross(const Vec3& rhs) {
        return Vec3 (
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        );
    }

    Vec3& Normalize() {
        return (*this) /= Mag();
    }
};

inline std::ostream& operator<<(std::ostream& os, const Vec3& vec) {
    return os << '[' << vec.x << ", " << vec.y << ", " << vec.z << ']';
}
