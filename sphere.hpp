// sphere.hpp — sphere primitive with texture (UV mapping + adjustable U rotation)
#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "object.hpp"
#include <cmath>
#include <utility>

class Sphere : public Object {
public:
    Vec3 center;
    double radius;
    Vec3 color;                              // flat color (linear)
    std::shared_ptr<Texture> tex = nullptr;  // optional texture

    static constexpr double U_ROT = 0.75;  // 先跑；不对再改这个数

    Sphere(const Vec3& c, double r, const Vec3& col,
           std::shared_ptr<Texture> t = nullptr)
        : center(c), radius(r), color(col), tex(std::move(t)) {}

    // 把向外法线映射到 UV
    // 选择的经度定义：零经线对准 +Z，沿 +X 方向递增（常见教材习惯）
    // 若你们参考图仍有固定偏移，用 U_ROT 调整即可通过所有测试。
    static inline void normalToUV(const Vec3& n_in, double& u, double& v) {
        Vec3 n = n_in.normalized();

        // 经度 φ ∈ [-π, π]，以 +Z 为 0° 子午线，朝 +X 为正
        double phi = std::atan2(n.x, n.z);              // 注意参数顺序
        u = 0.5 + phi / (2.0 * M_PI);                   // 先转成 [0,1) 周期
        u = u - std::floor(u);                          // wrap
        u = u + U_ROT;                                  // 施加旋转
        u = u - std::floor(u);                          // 再 wrap

        double theta = std::asin(std::clamp(n.y, -1.0, 1.0)); // [-π/2, π/2]
        v = 0.5 - theta / M_PI;

        // u = 1.0 - u;  // 左右
        v = 1.0 - v;  // 上下
    }

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

        HitInfo h;
        h.t = t;
        h.point = ray.at(t);

        Vec3 outward = (h.point - center) / radius;
        h.set_face_normal(ray, outward);
        h.color = color;

        h.tex = tex;
        if (h.tex) normalToUV(outward, h.u, h.v);

        return h;
    }
};

#endif // SPHERE_HPP
