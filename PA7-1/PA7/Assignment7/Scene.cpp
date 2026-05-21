//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"
#include <algorithm>

void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Intersection pos, lightPos;
    Vector3f L_dir = 0.0f;
    Vector3f L_indir = 0.0f;
    pos = intersect(ray);
    if (pos.happened){
        if (pos.m->hasEmission()){
            if (depth == 0) return pos.emit;
            else return Vector3f(0.0f);
        } 
        float pdf_light;
        sampleLight(lightPos, pdf_light);
        Vector3f wi, wo, ws, N, NN; //N是交点处的法线，NN是光源处的法线，wo是出射方向，wi是入射方向，ws是从交点指向光源的方向
        N = pos.normal;
        NN = lightPos.normal;
        // 单位化方向
        wo = (-ray.direction).normalized();
        wi = (pos.m->sample(wo, N)).normalized();
        ws = (lightPos.coords - pos.coords).normalized();

        // 直接光照贡献，判断是否被遮挡
        Ray p_x(pos.coords + N * EPSILON, ws);
        if (intersect(p_x).distance > ((lightPos.coords - pos.coords).norm()) - 0.005f){
            L_dir = lightPos.emit * pos.m->eval(wo, ws, N) * std::max(dotProduct(ws, N), 0.0f) * std::max(dotProduct(-ws, NN), 0.0f) / (std::pow((lightPos.coords - pos.coords).norm(), 2)) / pdf_light;
        }

        // 间接光照贡献，判断是否被遮挡
        Ray r(pos.coords + N * EPSILON, wi);
        float ksi = get_random_float();
        if (ksi > RussianRoulette) return L_dir;
        auto next_p = intersect(r);
        if (next_p.happened && !next_p.m->hasEmission()){
            if (pos.m->pdf(wo, wi, N) > EPSILON){
                L_indir = castRay(r, depth + 1) * pos.m->eval(wo, wi, N) * std::max(dotProduct(wi, N), 0.0f) / pos.m->pdf(wo, wi, N) / RussianRoulette;
            }
            else{
                L_indir = Vector3f(0.0f);
            }
        }
        return L_dir + L_indir;
    }
    return Vector3f(0.0f);
}