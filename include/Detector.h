#pragma once

#include <Eigen/Core>
#include "CameraBase.h"
#include "Detection.h"
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>

class Detector
{
public:
    Detector(CameraBase *camera);
    virtual std::vector<Detection> run() = 0;

protected:
    CameraBase *_camera;
};

class ClosestDetector : public Detector
{
public:
    ClosestDetector(CameraBase *camera) : Detector(camera) {}
    void run(std::vector<Detection> &detections) override;
};

class ObjectDetector : public Detector
{
public:
    ObjectDetector(CameraBase *camera, std::string modelPath, std::string labelPath, float CONFIDENCE_THRESHOLD);
    void run(std::vector<Detection> &detections) override;
    cv::dnn::Net net;

    const std::vector<cv::Scalar> colors = {cv::Scalar(255, 255, 0), cv::Scalar(0, 255, 0), cv::Scalar(0, 255, 255), cv::Scalar(255, 0, 0)};
    const float INPUT_WIDTH = 640.0;
    const float INPUT_HEIGHT = 640.0;
    const float SCORE_THRESHOLD = 0.2;
    const float NMS_THRESHOLD = 0.4;
    float confidenceThreshold;

    std::vector<std::string> class_list;
};