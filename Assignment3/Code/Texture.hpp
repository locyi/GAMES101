//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        // Clamp u and v to [0, 1] range
        u = std::max(0.0f, std::min(1.0f, u));
        v = std::max(0.0f, std::min(1.0f, v));
        
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        
        // Clamp to valid pixel indices
        u_img = std::max(0.0f, std::min((float)width - 1, u_img));
        v_img = std::max(0.0f, std::min((float)height - 1, v_img));
        
        auto color = image_data.at<cv::Vec3b>((int)v_img, (int)u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }
    Eigen::Vector3f  getColorBilinear(float u, float v)
    {
        u = std::max(0.0f, std::min(1.0f, u));
        v = std::max(0.0f, std::min(1.0f, v));
        
        auto u_img = u * width;
        auto v_img = (1 - v) * height;

        Eigen::Vector2i u00((int)floor(u_img),(int)floor(v_img));
        Eigen::Vector2i u01((int)floor(u_img),(int)ceil(v_img));
        Eigen::Vector2i u10((int)ceil(u_img),(int)floor(v_img));
        Eigen::Vector2i u11((int)ceil(u_img),(int)ceil(v_img));
        float s = u_img - u00.x();
        float t = v_img - u00.y();
        auto lerp = [](float x, float v0, float v1){
            return v0 + x*(v1 - v0);
        };
        Eigen::Vector3f uone;
        Eigen::Vector3f utwo;
        Eigen::Vector3f color;
        for (int i = 0; i < 3; i++){
            uone[i] = lerp(s, (image_data.at<cv::Vec3b>(u00.y(), u00.x()))[i], (image_data.at<cv::Vec3b>(u10.y(), u10.x()))[i]);
            utwo[i] = lerp(s, (image_data.at<cv::Vec3b>(u01.y(), u01.x()))[i], (image_data.at<cv::Vec3b>(u11.y(), u11.x()))[i]);
            color[i] = lerp(t, uone[i], utwo[i]);
        }
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

};
#endif //RASTERIZER_TEXTURE_H
