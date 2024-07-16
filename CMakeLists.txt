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

# list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/librealsense/2.55.1")
# find_package(realsense2 CONFIG REQUIRED)



set(REALSENSE_DIR "/opt/homebrew/Cellar/librealsense/2.55.1")
find_package(realsense2 CONFIG REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
link_directories(${SDL2_LIBDIR})
list(APPEND LIBS realsense2::realsense2)


add_executable(uChariotVision ${SRCS})
target_link_libraries(uChariotVision ${LIBS})
target_include_directories(uChariotVision PUBLIC ./include/)
