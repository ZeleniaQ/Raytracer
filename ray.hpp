#ifndef RAY_HPP
#define RAY_HPP

#include "vec3.hpp"

class Ray {
public:
    Vec3 origin;
    Vec3 direction;

    Ray() = default;
    Ray(const Vec3& origin, const Vec3& direction)
        : origin(origin), direction(direction.normalized()) {}

    Vec3 at(double t) const { return origin + direction * t; }
};

#endif // RAY_HPP