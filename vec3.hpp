#ifndef VEC3_HPP
#define VEC3_HPP

#include <cmath>
#include <iostream>

class Vec3 {
public:
    double x, y, z;

    Vec3(): x(0), y(0), z(0) {}
    Vec3(double x, double y, double z): x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.y, z - v.z); }
    Vec3 operator*(double s)     const { return Vec3(x * s, y * s, z * s); }
    Vec3 operator/(double s)     const { return Vec3(x / s, y / s, z / s); }

    Vec3& operator+=(const Vec3& v) { x += v.x; y += v.y; z += v.z; return *this; }
    Vec3& operator-=(const Vec3& v) { x -= v.x; y -= v.y; z -= v.z; return *this; }
    Vec3& operator*=(double s)      { x *= s; y *= s; z *= s; return *this; }
    Vec3& operator/=(double s)      { x /= s; y /= s; z /= s; return *this; }

    Vec3 operator-() const { return Vec3(-x, -y, -z); }

    double length() const { return std::sqrt(x*x + y*y + z*z); }
    double lengthSquared() const { return x*x + y*y + z*z; }

    Vec3 normalized() const {
        double len = length();
        return len == 0 ? Vec3(0, 0, 0) : (*this) / len;
    }

    double dot(const Vec3& v) const { return x*v.x + y*v.y + z*v.z; }
    Vec3 cross(const Vec3& v) const {
        return Vec3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    void print() const { std::cout << "(" << x << ", " << y << ", " << z << ")\n"; }
};

inline Vec3 operator*(double s, const Vec3& v) {
    return Vec3(s * v.x, s * v.y, s * v.z);
}

#endif // VEC3_HPP
