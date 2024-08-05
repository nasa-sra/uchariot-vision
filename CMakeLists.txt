cmake_minimum_required(VERSION 3.16)

project(UChariot-Vision)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread -std=c++17 -Wno-format")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..

set(SRCS 
src/main.cpp
src/Utils.cpp
src/MessageQueue.cpp
src/SimCamera.cpp
src/Display.cpp
src/Detector.cpp
src/Detection.cpp
)

set(LIBS)
find_package(CUDA REQUIRED)
include_directories(${CUDA_INCLUDE_DIRS})
find_package(Eigen3 CONFIG REQUIRED)
find_package( OpenCV CONFIG REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
find_package(jetson-utils)
find_package(jetson-inference)
find_package(VPI)
link_directories(/usr/lib/aarch64-linux-gnu/tegra)
set(LIBS Eigen3::Eigen ${OpenCV_LIBS} ${CUDA_LIBRARIES} jetson-inference vpi)

if (${SIMULATION})
message("Compiling for simulation")
add_compile_definitions(SIMULATION=true)
else()
list(APPEND SRCS src/Camera.cpp)

find_package(realsense2 REQUIRED)
    list(APPEND LIBS ${realsense2_LIBRARY})
endif()

add_executable(uChariotVision ${SRCS})
target_link_libraries(uChariotVision ${LIBS})
target_include_directories(uChariotVision PUBLIC ./include/)