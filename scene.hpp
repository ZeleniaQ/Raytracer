#ifndef SCENE_HPP
#define SCENE_HPP

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <algorithm>

#include "sphere.hpp"
#include "plane.hpp"
#include "triangle.hpp"
#include "texture.hpp"

struct Sun  { Vec3 dir; Vec3 color; };
struct Bulb { Vec3 pos; Vec3 color; };

class Scene {
public:
    int width = 0, height = 0;
    std::string filename;

    // camera state (保持你当前的简版，不动)
    Vec3 eye     = Vec3(0,0,0);
    Vec3 forward = Vec3(0,0,-1);  // length = zoom
    Vec3 up_hint = Vec3(0,1,0);

    int bounces    = 0;
    int aa_samples = 1;

    Vec3 current_color = Vec3(1,1,1);

    // --- texture state/cache ---
    std::shared_ptr<Texture> current_tex = nullptr; // "texture none" by default
    std::unordered_map<std::string, std::shared_ptr<Texture>> tex_cache;

    // xyz pool + per-xyz uv captured by texcoord
    std::vector<Vec3> xyz_vertices;
    std::vector<std::pair<double,double>> xyz_uvs;
    double cur_u = 0.0, cur_v = 0.0; // default texcoord

    // scene content
    std::vector<std::shared_ptr<Object>> objects;
    std::vector<Sun>  suns;
    std::vector<Bulb> bulbs;

    std::shared_ptr<Texture> getTexture(const std::string& path) {
        auto it = tex_cache.find(path);
        if (it != tex_cache.end()) return it->second;
        auto t = std::make_shared<Texture>();
        if (!t->load(path)) return nullptr;
        tex_cache[path] = t;
        return t;
    }

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
            }
            else if (cmd == "color") {
                double r,g,b; iss >> r >> g >> b; current_color = Vec3(r,g,b);
            }
            else if (cmd == "texture") {
                std::string name; iss >> name;
                if (name == "none") current_tex = nullptr;
                else current_tex = getTexture(name);
            }
            else if (cmd == "texcoord") {
                iss >> cur_u >> cur_v; // for subsequent xyz
            }
            else if (cmd == "xyz") {
                double x,y,z; iss >> x >> y >> z;
                xyz_vertices.emplace_back(x,y,z);
                xyz_uvs.emplace_back(cur_u, cur_v); // capture current texcoord
            }
            else if (cmd == "tri") {
                auto idx = [&](int k){
                    if (k > 0) return k-1;                    // 1-based
                    return (int)xyz_vertices.size() + k;      // negative from back
                };
                int i,j,k; iss >> i >> j >> k;
                int ia = idx(i), ib = idx(j), ic = idx(k);
                if (ia>=0 && ib>=0 && ic>=0 &&
                    ia<(int)xyz_vertices.size() &&
                    ib<(int)xyz_vertices.size() &&
                    ic<(int)xyz_vertices.size()) {
                    auto [u1,v1] = xyz_uvs[ia];
                    auto [u2,v2] = xyz_uvs[ib];
                    auto [u3,v3] = xyz_uvs[ic];
                    objects.emplace_back(std::make_shared<Triangle>(
                        xyz_vertices[ia], xyz_vertices[ib], xyz_vertices[ic],
                        current_color, u1,v1, u2,v2, u3,v3, current_tex
                    ));
                }
            }
            else if (cmd == "sphere") {
                double x,y,z,r; iss >> x >> y >> z >> r;
                // 现阶段球仍使用 flat color；需要贴图时可很快加（我们已支持 Texture）
                objects.emplace_back(std::make_shared<Sphere>(Vec3(x,y,z), r, current_color));
            }
            else if (cmd == "plane") {
                double A,B,C,D; iss >> A >> B >> C >> D;
                objects.emplace_back(std::make_shared<Plane>(A,B,C,D, current_color));
            }
            else if (cmd == "sun") {
                double x,y,z; iss >> x >> y >> z; suns.push_back(Sun{ Vec3(x,y,z), current_color });
            }
            else if (cmd == "bulb") {
                double x,y,z; iss >> x >> y >> z; bulbs.push_back(Bulb{ Vec3(x,y,z), current_color });
            }
            else if (cmd == "eye")     { double x,y,z; iss >> x >> y >> z; eye     = Vec3(x,y,z); }
            else if (cmd == "forward") { double x,y,z; iss >> x >> y >> z; forward = Vec3(x,y,z); }
            else if (cmd == "up")      { double x,y,z; iss >> x >> y >> z; up_hint = Vec3(x,y,z); }
            else if (cmd == "aa")      { int n; iss >> n; aa_samples = std::max(1, n); }
            else if (cmd == "bounces") { int d; iss >> d; bounces    = std::max(0, d); }
        }
        return true;
    }
};

#endif // SCENE_HPP
