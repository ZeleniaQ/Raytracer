#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "object.hpp"
#include <cmath>

class Sphere : public Object {
public:
    Vec3 center; double radius; Vec3 color;
    Sphere(const Vec3& c, double r, const Vec3& col) : center(c), radius(r), color(col) {}

    std::optional<HitInfo> intersect(const Ray& ray) const override {
        Vec3 oc = ray.origin - center;
        double a = ray.direction.lengthSquared();
        double b = 2.0 * oc.dot(ray.direction);
        double c = oc.lengthSquared() - radius * radius;
        double disc = b*b - 4*a*c;
        if (disc < 0) return std::nullopt;
        double s = std::sqrt(disc);
        double t = (-b - s) / (2*a);
        if (t <= 0) t = (-b + s) / (2*a);
        if (t <= 0) return std::nullopt;

        HitInfo h; h.t = t; h.point = ray.at(t);
        Vec3 outward = (h.point - center) / radius;
        h.set_face_normal(ray, outward);
        h.color = color;
        return h;
    }
};

#endif
