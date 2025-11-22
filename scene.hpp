// scene.hpp — scene parser and storage (camera, suns, bulbs, xyz/tri, plane/sphere)
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
#include "triangle.hpp"   // for xyz / tri

struct Sun  { Vec3 dir; Vec3 color; };   // directional light: L = normalize(dir) 指向光源
struct Bulb { Vec3 pos; Vec3 color; };   // point light: 1/d^2 衰减

class Scene {
public:
    // output
    int width = 0, height = 0;
    std::string filename;

    // camera state
    Vec3 eye     = Vec3(0,0,0);
    Vec3 forward = Vec3(0,0,-1);   // 不归一化：长度控制缩放
    Vec3 up_hint = Vec3(0,1,0);

    // render params
    int bounces    = 4;            // 反射/折射递归深度（预留）
    int aa_samples = 1;            // 抗锯齿采样数

    // state machine color for subsequent primitives/lights
    Vec3 current_color = Vec3(1,1,1);

    // scene content
    std::vector<std::shared_ptr<Object>> objects; // spheres / planes / triangles ...
    std::vector<Sun>  suns;                       // 方向光（允许多个）
    std::vector<Bulb> bulbs;                      // 点光

    // xyz vertex pool for tri
    std::vector<Vec3> xyz_vertices;

    bool loadFromFile(const std::string& path) {
        std::ifstream file(path);
        if (!file) return false;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::istringstream iss(line);
            std::string cmd; iss >> cmd;
            if (cmd.empty()) continue;

            if (cmd == "png") {
                iss >> width >> height >> filename;

            } else if (cmd == "color") {
                double r,g,b; iss >> r >> g >> b;
                current_color = Vec3(r,g,b);

            } else if (cmd == "sphere") {
                double x,y,z,r; iss >> x >> y >> z >> r;
                objects.emplace_back(std::make_shared<Sphere>(Vec3(x,y,z), r, current_color));

            } else if (cmd == "plane") {
                // plane A B C D  ->  Ax + By + Cz + D = 0
                double A,B,C,D; iss >> A >> B >> C >> D;
                objects.emplace_back(std::make_shared<Plane>(A,B,C,D, current_color));

            } else if (cmd == "sun") {
                double x,y,z; iss >> x >> y >> z;
                suns.push_back(Sun{ Vec3(x,y,z), current_color }); // dir=指向光源

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

            } else if (cmd == "xyz") {
                // store vertex position only; material captured at tri-time
                double x,y,z; iss >> x >> y >> z;
                xyz_vertices.emplace_back(x,y,z);

            } else if (cmd == "tri") {
                // 1-based indices; negative indices count from the back
                auto resolveIndex = [&](int idx)->int {
                    if (idx > 0) return idx - 1;                // 1..N -> 0..N-1
                    return int(xyz_vertices.size()) + idx;      // -1 -> last, etc.
                };
                int i,j,k; iss >> i >> j >> k;
                int ia = resolveIndex(i);
                int ib = resolveIndex(j);
                int ic = resolveIndex(k);
                if (ia >= 0 && ib >= 0 && ic >= 0 &&
                    ia < int(xyz_vertices.size()) &&
                    ib < int(xyz_vertices.size()) &&
                    ic < int(xyz_vertices.size()))
                {
                    objects.emplace_back(std::make_shared<Triangle>(
                        xyz_vertices[ia], xyz_vertices[ib], xyz_vertices[ic],
                        current_color   // capture material at tri invocation
                    ));
                }
                // 若索引非法：静默忽略该三角（也可打印警告）

            } // end command branches
        }
        return true;
    }
};

#endif // SCENE_HPP
