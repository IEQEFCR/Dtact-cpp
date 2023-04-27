#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "NumCpp.hpp"
#include "yaml-cpp/yaml.h"

#define _yaml YAML::Node

class Camera_Calibration {
   public:
    _yaml camera_setting, resolution_list;
    int resolution_option, camera_channel, img_width, img_height,
        cam_clb_img_number, corners_row, corners_column;
    std::string chessboard_images_path, prefix, image_format,
        parameter_saving_file;
    float square_size;
    bool optimization;

    Camera_Calibration(YAML::Node cfg) {
        camera_setting = cfg["camera_setting"];
        camera_channel = camera_setting["camera_channel"].as<int>();
        resolution_list = camera_setting["resolution_list"];
        resolution_option = camera_setting["resolution_option"].as<int>();
        img_width = resolution_list[resolution_option][0].as<int>();
        img_height = resolution_list[resolution_option][1].as<int>();

        _yaml camera_calibration = cfg["camera_calibration"];
        if (access("./images", 0) == -1)
            mkdir("./images", 0777);
        chessboard_images_path =
            camera_calibration["chessboard_images_path"].as<std::string>();
        if (access(chessboard_images_path.c_str(), 0) == -1) {
            mkdir(chessboard_images_path.c_str(), 0777);
        }

        cam_clb_img_number = camera_calibration["cam_clb_img_number"].as<int>();
        corners_row = camera_calibration["corners_row"].as<int>();
        corners_column = camera_calibration["corners_column"].as<int>();
        prefix = camera_calibration["prefix"].as<std::string>();
        image_format = camera_calibration["image_format"].as<std::string>();
        parameter_saving_file =
            chessboard_images_path +
            camera_calibration["parameter_saving_file"].as<std::string>();
        square_size = camera_calibration["square_size"].as<float>();
        optimization = camera_calibration["optimization"].as<bool>();
    }

    void collect_image() {
        cv::VideoCapture cap(camera_channel);
        if (cap.isOpened()) {
            std::cout << "Camera is opened." << std::endl;
        } else {
            std::cout << "Camera is not opened." << std::endl;
            exit(0);
        }

        cap.set(cv::CAP_PROP_FRAME_WIDTH, img_width);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, img_height);
        std::cout << "Ready to collect " << cam_clb_img_number << "images."
                  << std::endl;
        std::cout << "Press 's' to save or 'q' to quit !" << std::endl;

        cv::Mat frame;
        for (int i = 0; i < cam_clb_img_number; i++) {
            while (1) {
                cap >> frame;
                cv::imshow("frame", frame);
                char key = cv::waitKey(1);
                if (key == 's') {
                    std::string img_name = chessboard_images_path + "/" +
                                           prefix + std::to_string(i) + "." +
                                           image_format;
                    cv::imwrite(img_name, frame);
                    std::cout << "Image " << img_name << " saved." << std::endl;
                    break;
                } else if (key == 'q') {
                    std::cout << "Quit." << std::endl;
                    i = cam_clb_img_number;
                    break;
                }
            }
        }
        cap.release();
        cv::destroyAllWindows();
    }
    void calibration_process() {
        std::vector<std::vector<cv::Point3f>> object_points;
        std::vector<std::vector<cv::Point2f>> image_points;
        std::vector<cv::Point2f> corners;
        std::vector<cv::Point3f> obj;

        for (int i = 0; i < corners_row; i++) {
            for (int j = 0; j < corners_column; j++) {
                obj.push_back(cv::Point3f(i * square_size, j * square_size, 0));
            }
        }

        cv::Mat frame, gray;
        for (int i = 0; i < cam_clb_img_number; i++) {
            std::string img_name = chessboard_images_path + "/" + prefix +
                                   std::to_string(i) + "." + image_format;
            frame = cv::imread(img_name);
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
            bool found = cv::findChessboardCorners(
                gray, cv::Size(corners_row, corners_column), corners,
                cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
            if (found) {
                cv::cornerSubPix(
                    gray, corners, cv::Size(11, 11), cv::Size(-1, -1),
                    cv::TermCriteria(
                        cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30,
                        0.001));
                cv::drawChessboardCorners(frame,
                                          cv::Size(corners_row, corners_column),
                                          corners, found);
                cv::imshow("frame", frame);
                cv::waitKey(1);
                object_points.push_back(obj);
                image_points.push_back(corners);
            }
        }
        cv::destroyAllWindows();

        cv::Mat camera_matrix, dist_coeffs;
        std::vector<cv::Mat> rvecs, tvecs;
        cv::calibrateCamera(object_points, image_points,
                            cv::Size(img_width, img_height), camera_matrix,
                            dist_coeffs, rvecs, tvecs, optimization);
        std::cout << "Camera Matrix: " << std::endl
                  << camera_matrix << std::endl;
        std::cout << "Distortion Coefficients: " << std::endl
                  << dist_coeffs << std::endl;

        cv::FileStorage fs(parameter_saving_file, cv::FileStorage::WRITE);
        fs << "camera_matrix" << camera_matrix;
        fs << "distortion_coefficients" << dist_coeffs;
        fs.release();
    }
    ~Camera_Calibration() { return; }
};

int main() {
    _yaml config = YAML::LoadFile("../_config.yaml");
    Camera_Calibration camera_calibration(config);
    camera_calibration.collect_image();
    camera_calibration.calibration_process();
    return 0;
}
