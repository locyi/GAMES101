#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
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

    model = rotation_matrix * model;
    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.

    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    // TODO: Copy-paste your implementation from the previous assignment.
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f matrixPerspToOrth = Eigen::Matrix4f::Zero();
    Eigen::Matrix4f matrixOrth_translate = Eigen::Matrix4f::Zero();
    Eigen::Matrix4f matrixOrth_scale = Eigen::Matrix4f::Zero();
    float fov_rad = (eye_fov * MY_PI)/180;
    float x_scale_factor = 2*std::abs(zNear)*tan(fov_rad/2)*aspect_ratio; //right-left
    float y_scale_factor = 2*std::abs(zNear)*tan(fov_rad/2); //top-bottom
    float z_scale_factor = zNear-zFar; //far-near !!Opengl z axis is negative!!




    matrixPerspToOrth <<
        -zNear, 0, 0, 0,
        0, -zNear, 0, 0,
        0, 0, -(zNear + zFar), -zNear * zFar,
        0, 0, 1, 0;



    matrixOrth_translate << 
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, -(-zNear + -zFar)/2,
        0, 0, 0, 1;


    matrixOrth_scale <<
        2/x_scale_factor, 0, 0, 0,
        0, 2/y_scale_factor, 0, 0,
        0, 0, 2/z_scale_factor, 0,
        0, 0, 0, 1;

    projection = matrixOrth_scale * matrixOrth_translate * matrixPerspToOrth;


    return projection;
}


Eigen::Matrix4f get_rotation(Vector3f axis, float angle){
    float angle_rad = (angle * MY_PI)/180;
    Eigen::Matrix4f rotation = Eigen::Matrix4f::Zero();
    Eigen::Matrix4f identityMatrix = Eigen::Matrix4f::Identity();
    float x = axis[0];
    float y = axis[1];
    float z = axis[2];
    float Cos = cos(angle_rad);
    float Sin = sin(angle_rad);
    Eigen::Matrix4f axis_form_skew_symmetric = Eigen::Matrix4f::Zero(); //反对称矩阵
    axis_form_skew_symmetric(0,1) = -y;
    axis_form_skew_symmetric(0,2) = y;
    axis_form_skew_symmetric(1,0) = z;
    axis_form_skew_symmetric(1,2) = -x;
    axis_form_skew_symmetric(2,0) = -z;
    axis_form_skew_symmetric(2,1) = x;
    axis_form_skew_symmetric(3,3) = 1;
    Eigen::Vector4f axis_vec4d;
    axis_vec4d.block<3,1>(0,0) = axis;
    rotation = Cos * identityMatrix + (1-Cos)*axis_vec4d*axis_vec4d.transpose() + Sin*axis_form_skew_symmetric;

    return rotation;
}


int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4) {
            filename = std::string(argv[3]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        //r.set_model(get_rotation(Eigen::Vector3f(-1,5,0),angle));//旋转轴为（-1,5,0）
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        //r.set_model(get_rotation(Eigen::Vector3f(-1,5,0),angle));//旋转轴为（-1,5,0）
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
