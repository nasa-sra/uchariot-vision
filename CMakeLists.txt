cmake_minimum_required(VERSION 3.16)

project(UChariot-Vision)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O2 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)

add_compile_definitions(LINUX=true)

set(SRCS src/main.cpp src/Utils.cpp src/RotationEstimator.cpp src/Realsense.cpp src/Pipe.cpp)

set(LIBS)

find_package(Eigen3 CONFIG REQUIRED)
list(APPEND LIBS Eigen3::Eigen)

set(REALSENSE_DIR "/opt/homebrew/Cellar/librealsense/2.55.1")
find_package(realsense2 CONFIG REQUIRED)
list(APPEND LIBS realsense2::realsense2)

set(OPENCV_DIR "/opt/homebrew/Cellar/opencv/4.10.0")
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

# list(APPEND CMAKE_PREFIX_PATH "/usr/local/lib/cmake/opencv4")
# find_package(OpenCV CONFIG REQUIRED)
# list(APPEND LIBS ${OpenCV_LIBS})

add_executable(uChariotVision ${SRCS})
target_link_libraries(uChariotVision ${LIBS})
target_include_directories(uChariotVision PUBLIC ./include/)
