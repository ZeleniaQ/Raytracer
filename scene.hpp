#ifndef SCENE_HPP
#define SCENE_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include "sphere.hpp"
#include "plane.hpp"

struct Sun  { Vec3 dir;  Vec3 color; }; // L = normalize(dir) 指向光源
struct Bulb { Vec3 pos;  Vec3 color; }; // 点光 1/d^2 衰减

class Scene {
public:
    int width = 0, height = 0;
    std::string filename;

    // camera（Core 需求）
    Vec3 eye = Vec3(0,0,0);
    Vec3 forward = Vec3(0,0,-1); // 长度=缩放
    Vec3 up_hint = Vec3(0,1,0);

    int bounces = 4;            // 预留给反射/折射
    int aa_samples = 1;         // 抗锯齿采样数

    Vec3 current_color = Vec3(1,1,1);

    std::vector<std::shared_ptr<Object>> objects;
    std::vector<Sun>  suns;
    std::vector<Bulb> bulbs;

    bool loadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file) return false;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            std::string cmd; iss >> cmd; if (cmd.empty()) continue;

            if (cmd == "png") {
                iss >> width >> height >> filename;
            } else if (cmd == "color") {
                double r,g,b; iss >> r >> g >> b; current_color = Vec3(r,g,b);
            } else if (cmd == "sphere") {
                double x,y,z,r; iss >> x >> y >> z >> r;
                objects.emplace_back(std::make_shared<Sphere>(Vec3(x,y,z), r, current_color));
            } else if (cmd == "plane") {
                double A,B,C,D; iss >> A >> B >> C >> D;
                objects.emplace_back(std::make_shared<Plane>(A,B,C,D, current_color));
            } else if (cmd == "sun") {
                double x,y,z; iss >> x >> y >> z;
                suns.push_back(Sun{ Vec3(x,y,z), current_color }); // 指向光源
            } else if (cmd == "bulb") {
                double x,y,z; iss >> x >> y >> z;
                bulbs.push_back(Bulb{ Vec3(x,y,z), current_color });
            } else if (cmd == "eye") {
                double x,y,z; iss >> x >> y >> z; eye = Vec3(x,y,z);
            } else if (cmd == "forward") {
                double x,y,z; iss >> x >> y >> z; forward = Vec3(x,y,z);
            } else if (cmd == "up") {
                double x,y,z; iss >> x >> y >> z; up_hint = Vec3(x,y,z);
            } else if (cmd == "aa") {
                int n; iss >> n; aa_samples = std::max(1, n);
            } else if (cmd == "bounces") {
                int d; iss >> d; bounces = std::max(0, d);
            }
        }
        return true;
    }
};

#endif
