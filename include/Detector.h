#pragma once

#include <opencv2/opencv.hpp>
#include <Eigen/Core>

#include "Camera.h"

struct Detection {
    Eigen::Vector3d pos;
    std::string name;
    int pixelX, pixelY;
};

class Detector {
public:
    Detector(CameraBase* camera);
    virtual std::vector<Detection> run() = 0;

protected:

    CameraBase* _camera;

};

class ClosestDetector : public Detector {
public:
    ClosestDetector(CameraBase* camera) : Detector(camera) {}
    std::vector<Detection> run();
};
