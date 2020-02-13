cmake_minimum_required(VERSION 3.14)
project(saurons_eye)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV_DIR "C:/Program Files/opencv/opencv-master/build/install")
find_package (OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(saurons_eye test_retina.cpp)
target_link_libraries(saurons_eye ${OpenCV_LIBS})