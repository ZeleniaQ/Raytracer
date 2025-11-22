#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "object.hpp"
#include "texture.hpp"
#include <cmath>

class Triangle : public Object {
public:
    Vec3 a, b, c;           // positions
    Vec3 color;             // flat color when no texture
    // per-vertex uv captured at xyz time
    double ua, va, ub, vb, uc, vc;
    std::shared_ptr<Texture> tex; // optional

    Triangle(const Vec3& A, const Vec3& B, const Vec3& C,
             const Vec3& col,
             double uA, double vA, double uB, double vB, double uC, double vC,
             std::shared_ptr<Texture> T)
        : a(A), b(B), c(C), color(col),
          ua(uA), va(vA), ub(uB), vb(vB), uc(uC), vc(vC),
          tex(std::move(T)) {}

    std::optional<HitInfo> intersect(const Ray& ray) const override {
        constexpr double EPS = 1e-9;
        Vec3 e1 = b - a, e2 = c - a;
        Vec3 p  = ray.direction.cross(e2);
        double det = e1.dot(p);
        if (std::fabs(det) < EPS) return std::nullopt;
        double invDet = 1.0 / det;

        Vec3 tvec = ray.origin - a;
        double u = tvec.dot(p) * invDet;
        if (u < -EPS || u > 1.0 + EPS) return std::nullopt;

        Vec3 qvec = tvec.cross(e1);
        double v = ray.direction.dot(qvec) * invDet;
        if (v < -EPS || (u + v) > 1.0 + EPS) return std::nullopt;

        double t = e2.dot(qvec) * invDet;
        if (t <= EPS) return std::nullopt;

        HitInfo h;
        h.t = t;
        h.point = ray.at(t);
        Vec3 gn = e1.cross(e2).normalized();
        h.set_face_normal(ray, gn);
        h.color = color;
        h.tex = tex;

        if (h.tex) {
            double w = 1.0 - u - v;     // barycentric weights
            h.u = w*ua + u*ub + v*uc;
            h.v = w*va + u*vb + v*vc;

            // 关键：把三角形的 v 翻转一下，匹配你当前的 Texture::sample 约定
            // （Texture::sample 里用的是 iy = floor((1 - v) * h)）
            h.v = 1.0 - h.v;
        }
        return h;
    }
};
#endif // TRIANGLE_HPP
