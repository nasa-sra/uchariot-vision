#pragma once

#include <string>
#include <Eigen/Core>
#include <opencv2/opencv.hpp>

#include "Utils.h"

class Detection {
public:
    std::string name;
    Eigen::Vector3d pos;
    int x, y; // center

    void draw(cv::Mat frame);
    std::string toJsonStr();
};

class ObjectDetection : public Detection {
public:
    // float x1, y1, x2, y2; // corners
    int class_id;
    float confidence;
    cv::Rect box;

    void draw(cv::Mat frame);
    std::string toJsonStr();
};