#include "camera.h"

Camera::Camera(_yaml cfg) {
    camera_setting = cfg["camera_setting"];
    camera_channel = camera_setting["camera_channel"].as<int>();
    resolution_list = camera_setting["resolution_list"];
    resolution_option = camera_setting["resolution_option"].as<int>();
    raw_img_width = resolution_list[resolution_option][0].as<int>();
    raw_img_height = resolution_list[resolution_option][1].as<int>();
    fps = camera_setting["fps"].as<int>();
    cap.open(camera_channel);
    if (cap.isOpened())
        std::cout << "------Camera is open--------" << std::endl;
    else {
        std::cout << "Camera is not opened.\nPlease check channel" << std::endl;
        exit(0);
    }
    cap.set(cv::CAP_PROP_FRAME_WIDTH, raw_img_width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, raw_img_height);
    cap.set(cv::CAP_PROP_FPS, fps);

    // chessboard_images_path =
    //     cfg["camera_calibration"]["chessboard_images_path"].as<std::string>();
    // parameter_saving_file =
    //     chessboard_images_path +
    //     cfg["camera_calibration"]["parameter_saving_file"].as<std::string>();
    // _yaml camera_parameter = YAML::LoadFile(parameter_saving_file);
    // mtx = camera_parameter["camera_matrix"].as<cv::Mat>();
    // dist = camera_parameter["dist_coeff"].as<cv::Mat>();
    // newcameramtx = cv::getOptimalNewCameraMatrix(
    //     mtx, dist, cv::Size(raw_img_width, raw_img_height), 1,
    //     cv::Size(raw_img_width, raw_img_height), &roi);

    sensor_setting = cfg["sensor_setting"];
    crop_img_height = sensor_setting["crop_size"][0].as<int>();
    crop_img_width = sensor_setting["crop_size"][1].as<int>();
    surface_center_row = sensor_setting["surface_center"][0].as<int>();
    surface_center_col = sensor_setting["surface_center"][1].as<int>();
    width_begin = surface_center_col - crop_img_width / 2;
    width_end = surface_center_col + crop_img_width / 2;
    height_begin = surface_center_row - crop_img_height / 2;
    height_end = surface_center_row + crop_img_height / 2;
}
// inline ?
cv::Mat Camera::get_raw_image() {
    cv::Mat ret;
    cap >> ret;
    return ret;
}

cv::Mat Camera::rectify_image(cv::Mat img) {
    cv::Mat ret;
    cv::undistort(img, ret, mtx, dist, newcameramtx);
    return ret;
}

cv::Mat Camera::crop_image(cv::Mat img) {
    cv::Mat ret;
    std::cout << "OK \n";
    ret = img(
        cv::Rect(width_begin, height_begin, crop_img_width, crop_img_height));
    return ret;
}

cv::Mat Camera::rectify_crop_image(cv::Mat img) {
    cv::Mat ret;
    // ret = rectify_image(img);
    ret = crop_image(ret);
    return ret;
}

cv::Mat Camera::get_rectify_image() {
    cv::Mat ret;
    ret = get_raw_image();
    // ret = rectify_image(ret);
    return ret;
}

cv::Mat Camera::get_rectify_crop_image() {
    cv::Mat ret;
    ret = get_raw_image();
    // ret = rectify_crop_image(ret);
    return ret;
}

cv::Mat Camera::get_raw_avg_image(int avg_num) {
    while (1) {
        cap >> img_temp;

        // show img and croped img
        cv::imshow("img", img_temp);
        cv::imshow("crop_img", crop_image(img_temp));
        char c = cv::waitKey(1);
        if (c == 'y') {
            // destroy window
            cv::destroyWindow("img");
            cv::destroyWindow("crop_img");
            break;
        }
        if (c == 'q') {
            cv::destroyWindow("img");
            cv::destroyWindow("crop_img");
            exit(0);
        }
    }

    cv::Mat img_add = cv::Mat::zeros(img_temp.size(), img_temp.type());
    // 有必要用浮点数吗，感觉不用
    for (int i = 0; i < avg_num; i++) {
        cap >> img_temp;
        img_add += img_temp;
        // sleep 0.1s
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    img_add = img_add / avg_num;
    return img_add;
}

cv::Mat Camera::get_rectify_avg_image(int avg_num) {
    while (1) {
        cap >> img_temp;
        img_temp = rectify_image(img_temp);
        cv::imshow("img", img_temp);
        // cv::imshow("crop_img", crop_image(img_temp));
        char c = cv::waitKey(1);
        if (c == 'y') {
            // destroy window
            cv::destroyWindow("img");
            break;
        }
        if (c == 'q') {
            cv::destroyWindow("img");
            exit(0);
        }
    }
    cv::Mat img_add = cv::Mat::zeros(img_temp.size(), img_temp.type());
    // 有必要用浮点数吗，感觉不用
    for (int i = 0; i < avg_num; i++) {
        cap >> img_temp;
        img_add += img_temp;
        // sleep 0.1s
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    img_add = img_add / avg_num;
    return img_add;
}

cv::Mat Camera::get_rectify_crop_avg_image(int avg_num) {
    while (1) {
        cap >> img_temp;
        img_temp = rectify_crop_image(img_temp);
        cv::imshow("img", img_temp);
        char c = cv::waitKey(1);
        if (c == 'y') {
            // destroy window
            cv::destroyWindow("img");

            break;
        }
        if (c == 'q') {
            cv::destroyWindow("img");
            exit(0);
        }
    }
    cv::Mat img_add = cv::Mat::zeros(img_temp.size(), img_temp.type());
    // 有必要用浮点数吗，感觉不用
    for (int i = 0; i < avg_num; i++) {
        cap >> img_temp;
        img_add += img_temp;
        // sleep 0.1s
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    img_add = img_add / avg_num;
    return img_add;
}

cv::Mat Camera::img_list_avg_rectify(std::string list[], int img_num) {
    cv::Mat img_add = cv::imread(list[0]);
    for (int i = 0; i < img_num; i++)
        img_add += cv::imread(list[i]);
    img_add = img_add / img_num;
    img_add = rectify_image(img_add);
    return img_add;
}

Camera::~Camera() {
    std::cout << "now exit ";
    exit(0);
}
