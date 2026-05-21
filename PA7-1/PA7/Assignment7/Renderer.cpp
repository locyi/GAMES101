//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include "Scene.hpp"
#include "Renderer.hpp"
#include "thread"
#include <functional>
#include <atomic>

inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

// 子线程具体实现，传入像素分组，每个像素的采样数，eye_pos, framebuffer, scene, 当前线程编号t, 每个线程处理的行数rows
void Renderer::subThread(std::vector<Vector3f>& dirs, int spp,  Vector3f eye_pos, std::vector<Vector3f>& framebuffer, const Scene& scene, uint32_t t, uint32_t rows, std::atomic<uint32_t>& processedPixels){
    for (uint32_t j = 0; j < rows; j++){
        for (uint32_t k = 0; k < spp; k++){
            framebuffer[j + t  * rows] += scene.castRay(Ray(eye_pos, dirs[j]), 0) / spp;
        }
        {
            processedPixels++;
        }
    }
}
// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene)
{
    std::vector<Vector3f> framebuffer(scene.width * scene.height);

    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);
    int m = 0;

    // change the spp value to change sample ammount
    int spp = 32;
    int threadSum = 16;
    int pixelSum = scene.width * scene.height;
    uint32_t rows = pixelSum / threadSum;   // 每个线程处理的像素行数
    uint32_t cols = threadSum;  // 线程数量
    std::atomic<uint32_t> processedPixels(0); // 已处理的像素数量
    std::vector<std::vector<Vector3f>> temDirs(cols, std::vector<Vector3f>(rows));
    std::vector<Vector3f> dirs;
    std::cout << "SPP: " << spp << "\n";
    for (uint32_t j = 0; j < scene.height; ++j) {
        for (uint32_t i = 0; i < scene.width; ++i) {
            // generate primary ray direction
            float x = (2 * (i + 0.5) / (float)scene.width - 1) *
                      imageAspectRatio * scale;
            float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

            Vector3f dir = normalize(Vector3f(-x, y, 1));
            dirs.emplace_back(dir); // 将每个像素的ray direction存储到dirs中，后续分组传入子线程
            /*
            for (int k = 0; k < spp; k++){
                framebuffer[m] += scene.castRay(Ray(eye_pos, dir), 0) / spp;  
            }
            m++; 
            */
            
        }
    }

    std::vector<std::thread> threads;   // 存储子线程
    for (uint32_t t = 0; t < cols; t++){
        for (uint32_t group = 0; group < rows; group++){
            temDirs[t][group] = (dirs.at(t * rows + group));   // t是线程编号，group是线程内像素编号，将dirs中的ray direction分组存储到temDirs中
        }
        threads.emplace_back(subThread, std::ref(temDirs[t]), spp, eye_pos, std::ref(framebuffer), std::cref(scene), t, rows, std::ref(processedPixels));   // 创建子线程，传入分组后的ray direction以及其他参数
    }

    while (processedPixels < pixelSum) {   // 主线程监控渲染进度，直到所有像素处理完成
        {
            UpdateProgress((float)processedPixels / pixelSum); // 更新进度条
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 每100ms更新一次进度
    }

    for (auto& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }

    UpdateProgress(1.f);

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
