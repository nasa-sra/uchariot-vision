cmake_minimum_required(VERSION 3.16)

project(UChariot-Vision)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread -std=c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

# cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ..

set(SRCS 
    src/main.cpp
    src/Utils.cpp
    src/Pipe.cpp
    src/SimCamera.cpp
    src/Display.cpp
    src/Detector.cpp
)

find_package(Eigen3 CONFIG REQUIRED)
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
set(LIBS Eigen3::Eigen ${OpenCV_LIBS})

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