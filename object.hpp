#ifndef OBJECT_HPP
#define OBJECT_HPP

#include "ray.hpp"
#include "texture.hpp"
#include <optional>
#include <memory>

struct HitInfo {
    double t;
    Vec3 point;
    Vec3 normal;
    Vec3 color;                                // flat color (linear)
    bool front_face;

    // texture
    std::shared_ptr<Texture> tex = nullptr;    // if set, use it
    double u = 0.0, v = 0.0;                   // valid when tex != nullptr

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

#endif
