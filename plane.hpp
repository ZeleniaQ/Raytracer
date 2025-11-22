#ifndef PLANE_HPP
#define PLANE_HPP

#include "object.hpp"
#include <cmath>

class Plane : public Object {
public:
    // plane: n·x + D = 0
    Vec3 n; double D; Vec3 color;
    Plane(double A, double B, double C, double D, const Vec3& col)
        : n(Vec3(A,B,C).normalized()), D(D), color(col) {}

    std::optional<HitInfo> intersect(const Ray& ray) const override {
        double denom = n.dot(ray.direction);
        if (std::fabs(denom) < 1e-8) return std::nullopt; // parallel
        double t = -(n.dot(ray.origin) + D) / denom;      // n·(o + t d) + D = 0
        if (t <= 0) return std::nullopt;

        HitInfo h; h.t = t; h.point = ray.at(t);
        h.set_face_normal(ray, n);
        h.color = color;
        return h;
    }
};

#endif
