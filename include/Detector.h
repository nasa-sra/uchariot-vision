#pragma once

#include <Eigen/Core>

#include "CameraBase.h"
#include "Detection.h"

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
    std::vector<Detection> run();
};
