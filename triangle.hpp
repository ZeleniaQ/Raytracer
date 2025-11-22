// triangle.hpp — indexed triangle built from prior xyz commands
#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "object.hpp"
#include <cmath>

class Triangle : public Object {
public:
    Vec3 a, b, c;     // positions
    Vec3 color;       // captured at tri-time

    Triangle(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& col)
        : a(A), b(B), c(C), color(col) {}

    std::optional<HitInfo> intersect(const Ray& ray) const override {
        // Möller–Trumbore, with inclusive edges & tiny epsilon for robustness
        constexpr double EPS = 1e-9;
        Vec3 e1 = b - a;
        Vec3 e2 = c - a;

        Vec3 pvec = ray.direction.cross(e2);
        double det = e1.dot(pvec);
        if (std::fabs(det) < EPS) return std::nullopt; // parallel
        double invDet = 1.0 / det;

        Vec3 tvec = ray.origin - a;
        double u = tvec.dot(pvec) * invDet;
        if (u < -EPS || u > 1.0 + EPS) return std::nullopt;

        Vec3 qvec = tvec.cross(e1);
        double v = ray.direction.dot(qvec) * invDet;
        if (v < -EPS || (u + v) > 1.0 + EPS) return std::nullopt;

        double t = e2.dot(qvec) * invDet;
        if (t <= EPS) return std::nullopt;

        HitInfo h;
        h.t = t;
        h.point = ray.at(t);

        // geometric normal (double-sided with set_face_normal)
        Vec3 gn = e1.cross(e2).normalized();
        h.set_face_normal(ray, gn);
        h.color = color;
        return h;
    }
};

#endif // TRIANGLE_HPP
