cmake_minimum_required(VERSION 3.14)
project(saurons_eye)

set(OpenCV_DIR "C:/Program Files/opencv/opencv-master/build/install")
find_package (OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(saurons_eye main.cpp header.h)
target_link_libraries(saurons_eye ${OpenCV_LIBS})