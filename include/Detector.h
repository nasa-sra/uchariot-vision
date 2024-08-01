#pragma once

#include <Eigen/Core>
#include <opencv2/opencv.hpp>
#include <jetson-inference/detectNet.h>

#include "CameraBase.h"
#include "Detection.h"
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

    ObjectDetector(CameraBase *camera, std::string modelPath, std::string labelPath, float confThresh);
    ~ObjectDetector();

    void run(std::vector<Detection*> &detections) override;

private:
    detectNet* _net {nullptr};
    detectNet::Detection* _detections {nullptr};
    uchar3* _cudaImage {nullptr};

    const float INPUT_WIDTH = 300;
    const float INPUT_HEIGHT = 300;
    float _confidence_threshold;
    float _score_threshold;
    float _NMS_threshold;

    std::vector<std::string> _labels;
};