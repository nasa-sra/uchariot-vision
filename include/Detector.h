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
    virtual void run(std::vector<Detection*> &detections) = 0;

protected:
    CameraBase *_camera;
};

class ClosestDetector : public Detector
{
public:
    ClosestDetector(CameraBase *camera) : Detector(camera) {}
    void run(std::vector<Detection*> &detections) override;
};

class ObjectDetector : public Detector {
public:

    ObjectDetector(CameraBase *camera, std::string modelPath, std::string labelPath, float confThresh, float scoreThresh, float NMSThresh);
    void run(std::vector<Detection*> &detections) override;

private:

    cv::dnn::Net _net;
    const float INPUT_WIDTH = 640.0;
    const float INPUT_HEIGHT = 640.0;
    float _confidence_threshold;
    float _score_threshold;
    float _NMS_threshold;

    std::vector<std::string> _labels;
};