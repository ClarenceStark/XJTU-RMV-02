cmake_minimum_required(VERSION 3.10)
project(OpenCV_Project)

set(CMAKE_CXX_STANDARD 17)

set(OpenCV_DIR "/opt/homebrew/opt/opencv/include/opencv4")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(OpenCV_Project src/main.cpp src/box_painting.cpp)
target_link_libraries(OpenCV_Project ${OpenCV_LIBS})
