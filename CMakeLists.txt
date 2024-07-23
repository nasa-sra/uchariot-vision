cmake_minimum_required(VERSION 3.16)

project(UChariot-Vision)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..

set(SRCS 
    src/main.cpp
    src/Utils.cpp
    # src/RotationEstimator.cpp
    # src/Realsense.cpp
    src/Pipe.cpp
    src/Camera.cpp
    src/Display.cpp
)

find_package(Eigen3 CONFIG REQUIRED)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
find_package(realsense2 REQUIRED)

add_executable(uChariotVision ${SRCS})
target_link_libraries(uChariotVision Eigen3::Eigen ${OpenCV_LIBS} ${realsense2_LIBRARY})
target_include_directories(uChariotVision PUBLIC ./include/)