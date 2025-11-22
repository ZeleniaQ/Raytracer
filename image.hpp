#ifndef IMAGE_HPP
#define IMAGE_HPP

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

class Image {
    int w, h, ch;
    std::vector<unsigned char> pixels; // RGBA
public:
    Image(int width, int height): w(width), h(height), ch(4), pixels(width*height*4, 0) {}

    void setRGBA(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
        int idx = (y * w + x) * ch;
        pixels[idx+0]=r; pixels[idx+1]=g; pixels[idx+2]=b; pixels[idx+3]=a;
    }

    void setLinear(int x, int y, double lr, double lg, double lb, unsigned char a) {
        auto toByteSRGB = [](double v)->unsigned char {
            v = std::clamp(v, 0.0, 1.0);
            double s = (v <= 0.0031308) ? (12.92*v) : (1.055*std::pow(v, 1.0/2.4) - 0.055);
            int iv = (int)std::lround(s * 255.0);
            return (unsigned char)std::clamp(iv, 0, 255);
        };
        int idx = (y * w + x) * ch;
        pixels[idx+0]=toByteSRGB(lr);
        pixels[idx+1]=toByteSRGB(lg);
        pixels[idx+2]=toByteSRGB(lb);
        pixels[idx+3]=a; // alpha 不做 gamma
    }

    void save(const std::string& filename) {
        stbi_write_png(filename.c_str(), w, h, ch, pixels.data(), w*ch);
    }
};

#endif
