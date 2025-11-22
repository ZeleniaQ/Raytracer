#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "scene.hpp"
#include "image.hpp"
#include <algorithm>
#include <random>

class Renderer {
public:
    const Scene& scene;
    Renderer(const Scene& s) : scene(s) {}

    void render() {
        Image img(scene.width, scene.height);
        const int W = scene.width, H = scene.height;
        const int S = std::max(W, H);

        // camera basis (old style you had before)
        Vec3 f = scene.forward;
        Vec3 z = f * (1.0 / std::max(1e-12, f.length()));
        Vec3 r = z.cross(scene.up_hint).normalized();
        Vec3 u = r.cross(z).normalized();

        std::mt19937 rng(42);
        std::uniform_real_distribution<double> U(0.0, 1.0);
        constexpr double EPS = 1e-4;

        for (int y=0;y<H;++y){
            for (int x=0;x<W;++x){
                Vec3 accum(0,0,0);
                unsigned char outA = 0;

                for(int s=0; s<scene.aa_samples; ++s){
                    double jx = (scene.aa_samples==1)?0.5:U(rng);
                    double jy = (scene.aa_samples==1)?0.5:U(rng);
                    double sx = (2.0*(x+jx)-W)/(double)S;
                    double sy = (H-2.0*(y+jy))/(double)S;

                    Vec3 dir = r*sx + u*sy + z*f.length();
                    Ray ray(scene.eye, dir);

                    double tmin=1e30; std::optional<HitInfo> best;
                    for (const auto& obj: scene.objects){
                        auto h = obj->intersect(ray);
                        if (h && h->t>0.0 && h->t<tmin){ tmin=h->t; best=h; }
                    }
                    if (!best) continue;
                    outA = 255;

                    if (scene.suns.empty() && scene.bulbs.empty()) continue;

                    // --- texture or flat color ---
                    Vec3 base;
                    if (best->tex) base = best->tex->sample(best->u, best->v);
                    else           base = best->color;

                    Vec3 p=best->point, n=best->normal;
                    Vec3 radiance(0,0,0);

                    // suns
                    for (const auto& sun: scene.suns){
                        Vec3 L = (sun.dir).normalized();
                        Ray sh(p + n*EPS, L);
                        bool blocked=false;
                        for (const auto& obj: scene.objects){
                            auto shh = obj->intersect(sh);
                            if (shh && shh->t>0.0){ blocked=true; break; }
                        }
                        if (blocked) continue;
                        double ndotl = std::max(0.0, n.dot(L));
                        radiance += Vec3(base.x*sun.color.x, base.y*sun.color.y, base.z*sun.color.z) * ndotl;
                    }

                    // bulbs
                    for (const auto& b: scene.bulbs){
                        Vec3 toL = b.pos - p; double dist = toL.length();
                        if (dist < 1e-12) continue;
                        Vec3 L = toL / dist;
                        Ray sh(p + n*EPS, L);
                        bool blocked=false;
                        for (const auto& obj: scene.objects){
                            auto shh = obj->intersect(sh);
                            if (shh && shh->t>0.0 && shh->t < dist - EPS){ blocked=true; break; }
                        }
                        if (blocked) continue;
                        double ndotl=std::max(0.0, n.dot(L));
                        double att = 1.0 / std::max(1e-6, dist*dist);
                        radiance += Vec3(base.x*b.color.x, base.y*b.color.y, base.z*b.color.z) * (ndotl*att);
                    }

                    accum += radiance;
                }

                if (scene.aa_samples>1) accum /= (double)scene.aa_samples;
                if (outA==0) img.setRGBA(x,y,0,0,0,0);
                else         img.setLinear(x,y,accum.x,accum.y,accum.z,255);
            }
        }
        img.save(scene.filename);
    }
};

#endif
