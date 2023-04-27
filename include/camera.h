#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "NumCpp.hpp"
#include "yaml-cpp/yaml.h"

#define _yaml YAML::Node

class Camera {
   public:
    Camera(_yaml cfg);
    ~Camera();
    // video class
    cv::VideoCapture cap;
    _yaml camera_setting, resolution_list, sensor_setting;
    int resolution_option, camera_channel, raw_img_width, raw_img_height, fps;
    std::string chessboard_images_path, parameter_saving_file;
    cv::Mat mtx, dist, newcameramtx, img_temp;
    cv::Rect roi;
    int crop_img_width, crop_img_height;
    int surface_center_row, surface_center_col;
    int width_begin, width_end, height_begin, height_end;

    cv::Mat get_raw_image();
    cv::Mat rectify_image(cv::Mat img);
    cv::Mat crop_image(cv::Mat img);
    cv::Mat rectify_crop_image(cv::Mat img);
    cv::Mat get_rectify_image();
    cv::Mat get_rectify_crop_image();
    cv::Mat get_raw_avg_image(int avg_num = 10);
    cv::Mat get_rectify_avg_image(int avg_num = 10);
    cv::Mat get_rectify_crop_avg_image(int avg_num = 10);
    cv::Mat img_list_avg_rectify(std::string img_list[], int img_num);
};