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
        
    }

};
#endif //RASTERIZER_TEXTURE_H
