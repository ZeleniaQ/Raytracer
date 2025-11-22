#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

#include "vec3.hpp"

class Texture {
public:
    int w = 0, h = 0, comp = 0;          // comp = 1/3/4
    std::vector<unsigned char> data;     // original sRGB bytes

    Texture() = default;
    explicit Texture(const std::string& path) { load(path); }

    bool load(const std::string& path) {
        int x,y,n;
        stbi_uc* px = stbi_load(path.c_str(), &x, &y, &n, 4); // force RGBA
        if (!px) return false;
        w = x; h = y; comp = 4;
        data.assign(px, px + (size_t)w*h*4);
        stbi_image_free(px);
        return true;
    }

    // sample (u,v) in [0,1], wrap repeat; return **linear** Vec3
    Vec3 sample(double u, double v) const {
        if (w<=0 || h<=0) return Vec3(1,0,1); // debug magenta
        u -= std::floor(u);  // wrap
        v -= std::floor(v);
        int ix = std::clamp((int)std::floor(u * w), 0, w-1);
        int iy = std::clamp((int)std::floor((1.0 - v) * h), 0, h-1); // v top->bottom
        size_t idx = ((size_t)iy*w + ix) * 4;
        auto toLinear = [](double s)->double {
            s = std::clamp(s, 0.0, 1.0);
            return (s <= 0.04045) ? (s/12.92) : std::pow((s+0.055)/1.055, 2.4);
        };
        double sr = data[idx+0] / 255.0;
        double sg = data[idx+1] / 255.0;
        double sb = data[idx+2] / 255.0;
        return Vec3(toLinear(sr), toLinear(sg), toLinear(sb));
    }
};

#endif // TEXTURE_HPP
