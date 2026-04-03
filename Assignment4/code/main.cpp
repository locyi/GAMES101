#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    std::vector<cv::Point2f> mid_points;
    for (int i = 0; i < control_points.size()-1; i++){
        float x = control_points[i].x*(1-t) + control_points[i+1].x*t;
        float y = control_points[i].y*(1-t) + control_points[i+1].y*t;
        mid_points.push_back(cv::Point2f(x, y));
    }
    
    if (mid_points.size() == 1){
        return mid_points[0];
    }
    // TODO: Implement de Casteljau's algorithm
    return recursive_bezier(mid_points, t);

}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    for (double t = 0.0; t <= 1.0; t += 0.001){
        auto point = recursive_bezier(control_points, t);
        int centralPixel[2] = {(int)std::trunc(point.x),(int)std::trunc(point.y)};
        float nearbyPixel[4][2] = {{-0.5,0.5},{0.5,1.5},{1.5,0.5},{0.5,-0.5}};  //以中心点的上下左右中心坐标采样
        for (int i = 0; i < 4; i++){
            float instance = 0.0;
            float x = centralPixel[0] + nearbyPixel[i][0];
            float y = centralPixel[1] + nearbyPixel[i][1];
            instance = sqrtf(powf(point.x-x, 2)+powf(point.y-y, 2));
            int nP[2] = {centralPixel[0] + (int)(nearbyPixel[i][0] - 0.5f), centralPixel[1] + (int)(nearbyPixel[i][1] - 0.5f)}; //相邻坐标的绝对坐标
            if (window.at<cv::Vec3b>(nP[1], nP[0])[1] < 231.818*instance - 115.909) window.at<cv::Vec3b>(nP[1], nP[0])[1] = 231.818*instance - 115.909;
        }
        window.at<cv::Vec3b>(point.y, point.x)[1] = 255;
    }
    // TODO: Iterate through all t = 0 to t = 1 with small steps, and call de Casteljau's 
    // recursive Bezier algorithm.

}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("Bezier Curve", cv::WINDOW_AUTOSIZE);

    cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);

    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }

        if (control_points.size() == 4) 
        {
            //naive_bezier(control_points, window);
            bezier(control_points, window);

            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }

        cv::imshow("Bezier Curve", window);
        key = cv::waitKey(20);
    }

return 0;
}
