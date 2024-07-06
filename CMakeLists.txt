cmake_minimum_required(VERSION 3.18)

project(UChariot-Vision)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-O2 -pthread")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set (CMAKE_CXX_STANDARD 17)

add_compile_definitions(LINUX=true)

set(SRCS src/main.cpp src/Utils.cpp)

set(LIBS)

#find_package(Eigen3 CONFIG REQUIRED)
#list(APPEND LIBS Eigen3::Eigen)

add_executable(uChariotVision ${SRCS})
target_link_libraries(uChariotVision ${LIBS})
target_include_directories(uChariotVision PUBLIC ./include/)
