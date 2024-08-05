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

    virtual void draw(cv::Mat frame);
    virtual std::string toJsonStr();
};

class ObjectDetection : public Detection {
public:
    int class_id;
    float confidence;
    float x1, y1, x2, y2; // corners
    float width, height;
    cv::Rect box;

    void draw(cv::Mat frame) override;
    std::string toJsonStr() override;
};