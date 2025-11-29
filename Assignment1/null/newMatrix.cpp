#include <iostream>
#include </usr/include/eigen3/Eigen/Dense>

constexpr double MY_PI = 3.1415926;
Eigen::Matrix4f get_model_matrix(float rotation_angle);


class Box
{
   public:
      double length;   // 长度
      double breadth;  // 宽度
      double height;   // 高度
      // 成员函数声明
      double get(void);
      void set( double len, double bre, double hei );
};

double Box::get(void)
{
    return length * breadth * height;
}

int main() {
    Eigen::Matrix4f model;
    model = get_model_matrix(90);
    std::cout << model << std::endl;

    Box Box1;
    Box1.breadth = 1;
    Box1.height = 2;
    Box1.length = 6;
    std::cout << "Box1 volume:" << Box1.get() << std::endl;
    return 0;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    float algle_rad = (rotation_angle * MY_PI)/180;
    Eigen::Matrix4f rotation_matrix = Eigen::Matrix4f::Zero();
    rotation_matrix(0,0) = cos(algle_rad);
    rotation_matrix(1,0) = sin(algle_rad);
    rotation_matrix(0,1) = -sin(algle_rad);
    rotation_matrix(1,1) = cos(algle_rad);
    rotation_matrix(2,2) = 1;
    rotation_matrix(3,3) = 1;
    model = rotation_matrix.cast<float>() * model;

    Eigen::Vector3f axis = Eigen::Vector3f::Zero();
    axis << 1.0f, 2.0f, 4.0f;
    std::cout << axis << std::endl;
    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.

    return model;
}




