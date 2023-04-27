#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "NumCpp.hpp"
#include "camera.h"
#include "yaml-cpp/yaml.h"

#define out std::cout

int main() {
    _yaml cfg = YAML::LoadFile("../_config.yaml");
    Camera camera(cfg);
    // out << camera.width_begin << " " << camera.width_end << " "
    //     << camera.height_begin << " " << camera.height_end << "\n";
    // out << camera.raw_img_width << " " << camera.raw_img_height << "\n";
    cv::Mat img;
    camera.cap >> img;
    out << img.size() << "\n";
    cv::imshow("img", img);
    cv::waitKey(0);
    return 0;
}