cmake_minimum_required(VERSION 3.16.3)

project(Dtact)

# add_executable(_0_Camera_Calibration src/_0_Camera_Calibration.cpp)
add_executable(_3_3D_Reconstruction src/_3_3D_Reconstruction.cpp src/camera.cpp)

include_directories(include)

find_package(NumCpp 2.10.1 REQUIRED)
find_package(OpenCV 4.6.0 REQUIRED)
find_package(yaml-cpp REQUIRED)

# target_link_libraries(_0_Camera_Calibration NumCpp::NumCpp)
# target_link_libraries(_0_Camera_Calibration ${OpenCV_LIBS})
# target_link_libraries(_0_Camera_Calibration yaml-cpp)
target_link_libraries(_3_3D_Reconstruction NumCpp::NumCpp ${OpenCV_LIBS} yaml-cpp)

# target_include_directories(_3_3D_Reconstruction PUBLIC include)