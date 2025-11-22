#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "ray.hpp"
#include <optional>
#include <memory>

class Texture; // forward declare to avoid include cycle

struct HitInfo {
    double t;
    Vec3 point;
    Vec3 normal;
    Vec3 color;
    bool  front_face;

    // --- texture payload for triangles/spheres (optional) ---
    std::shared_ptr<Texture> tex = nullptr;
    double u = 0.0, v = 0.0;  // valid iff tex != nullptr

    void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        front_face = r.direction.dot(outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Object {
public:
    virtual ~Object() = default;
    virtual std::optional<HitInfo> intersect(const Ray& ray) const = 0;
};

#endif // OBJECT_HPP
